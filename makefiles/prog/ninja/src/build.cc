// Copyright 2011 Google Inc. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "build.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#endif

#include "build_log.h"
#include "disk_interface.h"
#include "graph.h"
#include "state.h"
#include "subprocess.h"
#include "util.h"

BuildStatus::BuildStatus(const BuildConfig& config)
    : config_(config),
      start_time_millis_(GetTimeMillis()),
      started_edges_(0), finished_edges_(0), total_edges_(0),
      have_blank_line_(true), progress_status_format_(NULL) {
#ifndef _WIN32
  const char* term = getenv("TERM");
  smart_terminal_ = isatty(1) && term && string(term) != "dumb";
#else
  // Disable output buffer.  It'd be nice to use line buffering but
  // MSDN says: "For some systems, [_IOLBF] provides line
  // buffering. However, for Win32, the behavior is the same as _IOFBF
  // - Full Buffering."
  setvbuf(stdout, NULL, _IONBF, 0);
  console_ = GetStdHandle(STD_OUTPUT_HANDLE);
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  smart_terminal_ = GetConsoleScreenBufferInfo(console_, &csbi);
#endif

  // Don't do anything fancy in verbose mode.
  if (config_.verbosity != BuildConfig::NORMAL)
    smart_terminal_ = false;

  progress_status_format_ = getenv("NINJA_STATUS");
  if (!progress_status_format_)
    progress_status_format_ = "[%s/%t] ";
}

void BuildStatus::PlanHasTotalEdges(int total) {
  total_edges_ = total;
}

void BuildStatus::BuildEdgeStarted(Edge* edge) {
  int start_time = (int)(GetTimeMillis() - start_time_millis_);
  running_edges_.insert(make_pair(edge, start_time));
  ++started_edges_;

  PrintStatus(edge);
}

void BuildStatus::BuildEdgeFinished(Edge* edge,
                                    bool success,
                                    const string& output,
                                    int* start_time,
                                    int* end_time) {
  int64_t now = GetTimeMillis();
  ++finished_edges_;

  RunningEdgeMap::iterator i = running_edges_.find(edge);
  *start_time = i->second;
  *end_time = (int)(now - start_time_millis_);
  running_edges_.erase(i);

  if (config_.verbosity == BuildConfig::QUIET)
    return;

  if (smart_terminal_)
    PrintStatus(edge);

  if (!success || !output.empty()) {
    if (smart_terminal_)
      printf("\n");

    // Print the command that is spewing before printing its output.
    if (!success)
      printf("FAILED: %s\n", edge->EvaluateCommand().c_str());

    // ninja sets stdout and stderr of subprocesses to a pipe, to be able to
    // check if the output is empty. Some compilers, e.g. clang, check
    // isatty(stderr) to decide if they should print colored output.
    // To make it possible to use colored output with ninja, subprocesses should
    // be run with a flag that forces them to always print color escape codes.
    // To make sure these escape codes don't show up in a file if ninja's output
    // is piped to a file, ninja strips ansi escape codes again if it's not
    // writing to a |smart_terminal_|.
    // (Launching subprocesses in pseudo ttys doesn't work because there are
    // only a few hundred available on some systems, and ninja can launch
    // thousands of parallel compile commands.)
    // TODO: There should be a flag to disable escape code stripping.
    string final_output;
    if (!smart_terminal_)
      final_output = StripAnsiEscapeCodes(output);
    else
      final_output = output;

    if (!final_output.empty())
      printf("%s", final_output.c_str());

    have_blank_line_ = true;
  }
}

void BuildStatus::BuildFinished() {
  if (smart_terminal_ && !have_blank_line_)
    printf("\n");
}

string BuildStatus::FormatProgressStatus(const char* progress_status_format) const {
  string out;
  char buf[32];
  for (const char* s = progress_status_format; *s != '\0'; ++s) {
    if (*s == '%') {
      ++s;
      switch (*s) {
      case '%':
        out.push_back('%');
        break;

        // Started edges.
      case 's':
        snprintf(buf, sizeof(buf), "%d", started_edges_);
        out += buf;
        break;

        // Total edges.
      case 't':
        snprintf(buf, sizeof(buf), "%d", total_edges_);
        out += buf;
        break;

        // Running edges.
      case 'r':
        snprintf(buf, sizeof(buf), "%d", started_edges_ - finished_edges_);
        out += buf;
        break;

        // Unstarted edges.
      case 'u':
        snprintf(buf, sizeof(buf), "%d", total_edges_ - started_edges_);
        out += buf;
        break;

        // Finished edges.
      case 'f':
        snprintf(buf, sizeof(buf), "%d", finished_edges_);
        out += buf;
        break;

      default: {
        Fatal("unknown placeholder '%%%c' in $NINJA_STATUS", *s);
        return "";
      }
      }
    } else {
      out.push_back(*s);
    }
  }

  return out;
}

void BuildStatus::PrintStatus(Edge* edge) {
  if (config_.verbosity == BuildConfig::QUIET)
    return;

  bool force_full_command = config_.verbosity == BuildConfig::VERBOSE;

  string to_print = edge->GetDescription();
  if (to_print.empty() || force_full_command)
    to_print = edge->EvaluateCommand();

#ifdef _WIN32
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  GetConsoleScreenBufferInfo(console_, &csbi);
#endif

  if (smart_terminal_) {
#ifndef _WIN32
    printf("\r");  // Print over previous line, if any.
#else
    csbi.dwCursorPosition.X = 0;
    SetConsoleCursorPosition(console_, csbi.dwCursorPosition);
#endif
  }

  to_print = FormatProgressStatus(progress_status_format_) + to_print;

  if (smart_terminal_ && !force_full_command) {
    const int kMargin = 3;  // Space for "...".
#ifndef _WIN32
    // Limit output to width of the terminal if provided so we don't cause
    // line-wrapping.
    winsize size;
    if ((ioctl(0, TIOCGWINSZ, &size) == 0) && size.ws_col) {
      if (to_print.size() + kMargin > size.ws_col) {
        int elide_size = (size.ws_col - kMargin) / 2;
        to_print = to_print.substr(0, elide_size)
          + "..."
          + to_print.substr(to_print.size() - elide_size, elide_size);
      }
    }
#else
    // Don't use the full width or console will move to next line.
    size_t width = static_cast<size_t>(csbi.dwSize.X) - 1;
    if (to_print.size() + kMargin > width) {
      int elide_size = (width - kMargin) / 2;
      to_print = to_print.substr(0, elide_size)
        + "..."
        + to_print.substr(to_print.size() - elide_size, elide_size);
    }
#endif
  }

  printf("%s", to_print.c_str());

  if (smart_terminal_ && !force_full_command) {
#ifndef _WIN32
    printf("\x1B[K");  // Clear to end of line.
    fflush(stdout);
    have_blank_line_ = false;
#else
    // Clear to end of line.
    GetConsoleScreenBufferInfo(console_, &csbi);
    int num_spaces = csbi.dwSize.X - 1 - csbi.dwCursorPosition.X;
    printf("%*s", num_spaces, "");
    have_blank_line_ = false;
#endif
  } else {
    printf("\n");
  }
}

Plan::Plan() : command_edges_(0), wanted_edges_(0) {}

bool Plan::AddTarget(Node* node, string* err) {
  vector<Node*> stack;
  return AddSubTarget(node, &stack, err);
}

bool Plan::AddSubTarget(Node* node, vector<Node*>* stack, string* err) {
  Edge* edge = node->in_edge();
  if (!edge) {  // Leaf node.
    if (node->dirty()) {
      string referenced;
      if (!stack->empty())
        referenced = ", needed by '" + stack->back()->path() + "',";
      *err = "'" + node->path() + "'" + referenced + " missing "
             "and no known rule to make it";
    }
    return false;
  }

  if (CheckDependencyCycle(node, stack, err))
    return false;

  if (edge->outputs_ready())
    return false;  // Don't need to do anything.

  // If an entry in want_ does not already exist for edge, create an entry which
  // maps to false, indicating that we do not want to build this entry itself.
  pair<map<Edge*, bool>::iterator, bool> want_ins =
    want_.insert(make_pair(edge, false));
  bool& want = want_ins.first->second;

  // If we do need to build edge and we haven't already marked it as wanted,
  // mark it now.
  if (node->dirty() && !want) {
    want = true;
    ++wanted_edges_;
    if (edge->AllInputsReady())
      ready_.insert(edge);
    if (!edge->is_phony())
      ++command_edges_;
  }

  if (!want_ins.second)
    return true;  // We've already processed the inputs.

  stack->push_back(node);
  for (vector<Node*>::iterator i = edge->inputs_.begin();
       i != edge->inputs_.end(); ++i) {
    if (!AddSubTarget(*i, stack, err) && !err->empty())
      return false;
  }
  assert(stack->back() == node);
  stack->pop_back();

  return true;
}

bool Plan::CheckDependencyCycle(Node* node, vector<Node*>* stack, string* err) {
  vector<Node*>::reverse_iterator ri =
      find(stack->rbegin(), stack->rend(), node);
  if (ri == stack->rend())
    return false;

  // Add this node onto the stack to make it clearer where the loop
  // is.
  stack->push_back(node);

  vector<Node*>::iterator start = find(stack->begin(), stack->end(), node);
  *err = "dependency cycle: ";
  for (vector<Node*>::iterator i = start; i != stack->end(); ++i) {
    if (i != start)
      err->append(" -> ");
    err->append((*i)->path());
  }
  return true;
}

Edge* Plan::FindWork() {
  if (ready_.empty())
    return NULL;
  set<Edge*>::iterator i = ready_.begin();
  Edge* edge = *i;
  ready_.erase(i);
  return edge;
}

void Plan::EdgeFinished(Edge* edge) {
  map<Edge*, bool>::iterator i = want_.find(edge);
  assert(i != want_.end());
  if (i->second)
    --wanted_edges_;
  want_.erase(i);
  edge->outputs_ready_ = true;

  // Check off any nodes we were waiting for with this edge.
  for (vector<Node*>::iterator i = edge->outputs_.begin();
       i != edge->outputs_.end(); ++i) {
    NodeFinished(*i);
  }
}

void Plan::NodeFinished(Node* node) {
  // See if we we want any edges from this node.
  for (vector<Edge*>::const_iterator i = node->out_edges().begin();
       i != node->out_edges().end(); ++i) {
    map<Edge*, bool>::iterator want_i = want_.find(*i);
    if (want_i == want_.end())
      continue;

    // See if the edge is now ready.
    if ((*i)->AllInputsReady()) {
      if (want_i->second) {
        ready_.insert(*i);
      } else {
        // We do not need to build this edge, but we might need to build one of
        // its dependents.
        EdgeFinished(*i);
      }
    }
  }
}

void Plan::CleanNode(BuildLog* build_log, Node* node) {
  node->set_dirty(false);

  for (vector<Edge*>::const_iterator ei = node->out_edges().begin();
       ei != node->out_edges().end(); ++ei) {
    // Don't process edges that we don't actually want.
    map<Edge*, bool>::iterator want_i = want_.find(*ei);
    if (want_i == want_.end() || !want_i->second)
      continue;

    // If all non-order-only inputs for this edge are now clean,
    // we might have changed the dirty state of the outputs.
    vector<Node*>::iterator begin = (*ei)->inputs_.begin(),
                            end = (*ei)->inputs_.end() - (*ei)->order_only_deps_;
    if (find_if(begin, end, mem_fun(&Node::dirty)) == end) {
      // Recompute most_recent_input and command.
      TimeStamp most_recent_input = 1;
      for (vector<Node*>::iterator ni = begin; ni != end; ++ni)
        if ((*ni)->mtime() > most_recent_input)
          most_recent_input = (*ni)->mtime();
      string command = (*ei)->EvaluateCommand(true);

      // Now, recompute the dirty state of each output.
      bool all_outputs_clean = true;
      for (vector<Node*>::iterator ni = (*ei)->outputs_.begin();
           ni != (*ei)->outputs_.end(); ++ni) {
        if (!(*ni)->dirty())
          continue;

        if ((*ei)->RecomputeOutputDirty(build_log, most_recent_input, NULL, command,
                                        *ni)) {
          (*ni)->MarkDirty();
          all_outputs_clean = false;
        } else {
          CleanNode(build_log, *ni);
        }
      }

      // If we cleaned all outputs, mark the node as not wanted.
      if (all_outputs_clean) {
        want_i->second = false;
        --wanted_edges_;
        if (!(*ei)->is_phony())
          --command_edges_;
      }
    }
  }
}

void Plan::Dump() {
  printf("pending: %d\n", (int)want_.size());
  for (map<Edge*, bool>::iterator i = want_.begin(); i != want_.end(); ++i) {
    if (i->second)
      printf("want ");
    i->first->Dump();
  }
  printf("ready: %d\n", (int)ready_.size());
}

struct RealCommandRunner : public CommandRunner {
  explicit RealCommandRunner(const BuildConfig& config) : config_(config) {}
  virtual ~RealCommandRunner() {}
  virtual bool CanRunMore();
  virtual bool StartCommand(Edge* edge);
  virtual Edge* WaitForCommand(ExitStatus* status, string* output);
  virtual vector<Edge*> GetActiveEdges();
  virtual void Abort();

  const BuildConfig& config_;
  SubprocessSet subprocs_;
  map<Subprocess*, Edge*> subproc_to_edge_;
};

vector<Edge*> RealCommandRunner::GetActiveEdges() {
  vector<Edge*> edges;
  for (map<Subprocess*, Edge*>::iterator i = subproc_to_edge_.begin();
       i != subproc_to_edge_.end(); ++i)
    edges.push_back(i->second);
  return edges;
}

void RealCommandRunner::Abort() {
  subprocs_.Clear();
}

bool RealCommandRunner::CanRunMore() {
  return ((int)subprocs_.running_.size()) < config_.parallelism
    && ((subprocs_.running_.size() == 0 || config_.max_load_average <= 0.0f)
        || GetLoadAverage() < config_.max_load_average);
}

bool RealCommandRunner::StartCommand(Edge* edge) {
  string command = edge->EvaluateCommand();
  Subprocess* subproc = subprocs_.Add(command);
  if (!subproc)
    return false;
  subproc_to_edge_.insert(make_pair(subproc, edge));

  return true;
}

Edge* RealCommandRunner::WaitForCommand(ExitStatus* status, string* output) {
  Subprocess* subproc;
  while ((subproc = subprocs_.NextFinished()) == NULL) {
    bool interrupted = subprocs_.DoWork();
    if (interrupted) {
      *status = ExitInterrupted;
      return 0;
    }
  }

  *status = subproc->Finish();
  *output = subproc->GetOutput();

  map<Subprocess*, Edge*>::iterator i = subproc_to_edge_.find(subproc);
  Edge* edge = i->second;
  subproc_to_edge_.erase(i);

  delete subproc;
  return edge;
}

/// A CommandRunner that doesn't actually run the commands.
struct DryRunCommandRunner : public CommandRunner {
  virtual ~DryRunCommandRunner() {}
  virtual bool CanRunMore() {
    return true;
  }
  virtual bool StartCommand(Edge* edge) {
    finished_.push(edge);
    return true;
  }
  virtual Edge* WaitForCommand(ExitStatus* status, string* /* output */) {
    if (finished_.empty()) {
      *status = ExitFailure;
      return NULL;
    }
    *status = ExitSuccess;
    Edge* edge = finished_.front();
    finished_.pop();
    return edge;
  }

  queue<Edge*> finished_;
};

Builder::Builder(State* state, const BuildConfig& config)
    : state_(state), config_(config) {
  disk_interface_ = new RealDiskInterface;
  status_ = new BuildStatus(config);
  log_ = state->build_log_;
}

Builder::~Builder() {
  Cleanup();
}

void Builder::Cleanup() {
  if (command_runner_.get()) {
    vector<Edge*> active_edges = command_runner_->GetActiveEdges();
    command_runner_->Abort();

    for (vector<Edge*>::iterator i = active_edges.begin();
         i != active_edges.end(); ++i) {
      bool has_depfile = !(*i)->rule_->depfile().empty();
      for (vector<Node*>::iterator ni = (*i)->outputs_.begin();
           ni != (*i)->outputs_.end(); ++ni) {
        // Only delete this output if it was actually modified.  This is
        // important for things like the generator where we don't want to
        // delete the manifest file if we can avoid it.  But if the rule
        // uses a depfile, always delete.  (Consider the case where we
        // need to rebuild an output because of a modified header file
        // mentioned in a depfile, and the command touches its depfile
        // but is interrupted before it touches its output file.)
        if (has_depfile ||
            (*ni)->mtime() != disk_interface_->Stat((*ni)->path()))
          disk_interface_->RemoveFile((*ni)->path());
      }
      if (has_depfile)
        disk_interface_->RemoveFile((*i)->EvaluateDepFile());
    }
  }
}

Node* Builder::AddTarget(const string& name, string* err) {
  Node* node = state_->LookupNode(name);
  if (!node) {
    *err = "unknown target: '" + name + "'";
    return NULL;
  }
  if (!AddTarget(node, err))
    return NULL;
  return node;
}

bool Builder::AddTarget(Node* node, string* err) {
  node->StatIfNecessary(disk_interface_);
  if (Edge* in_edge = node->in_edge()) {
    if (!in_edge->RecomputeDirty(state_, disk_interface_, err))
      return false;
    if (in_edge->outputs_ready())
      return true;  // Nothing to do.
  }

  if (!plan_.AddTarget(node, err))
    return false;

  return true;
}

bool Builder::AlreadyUpToDate() const {
  return !plan_.more_to_do();
}

bool Builder::Build(string* err) {
  assert(!AlreadyUpToDate());

  status_->PlanHasTotalEdges(plan_.command_edge_count());
  int pending_commands = 0;
  int failures_allowed = config_.failures_allowed;

  // Set up the command runner if we haven't done so already.
  if (!command_runner_.get()) {
    if (config_.dry_run)
      command_runner_.reset(new DryRunCommandRunner);
    else
      command_runner_.reset(new RealCommandRunner(config_));
  }

  // This main loop runs the entire build process.
  // It is structured like this:
  // First, we attempt to start as many commands as allowed by the
  // command runner.
  // Second, we attempt to wait for / reap the next finished command.
  // If we can do neither of those, the build is stuck, and we report
  // an error.
  while (plan_.more_to_do()) {
    // See if we can start any more commands.
    if (failures_allowed && command_runner_->CanRunMore()) {
      if (Edge* edge = plan_.FindWork()) {
        if (!StartEdge(edge, err)) {
          status_->BuildFinished();
          return false;
        }

        if (edge->is_phony())
          FinishEdge(edge, true, "");
        else
          ++pending_commands;

        // We made some progress; go back to the main loop.
        continue;
      }
    }

    // See if we can reap any finished commands.
    if (pending_commands) {
      ExitStatus status;
      string output;
      Edge* edge = command_runner_->WaitForCommand(&status, &output);
      if (edge && status != ExitInterrupted) {
        bool success = (status == ExitSuccess);
        --pending_commands;
        FinishEdge(edge, success, output);
        if (!success) {
          if (failures_allowed)
            failures_allowed--;
        }

        // We made some progress; start the main loop over.
        continue;
      }

      if (status == ExitInterrupted) {
        status_->BuildFinished();
        *err = "interrupted by user";
        return false;
      }
    }

    // If we get here, we can neither enqueue new commands nor are any running.
    if (pending_commands) {
      status_->BuildFinished();
      *err = "stuck: pending commands but none to wait for? [this is a bug]";
      return false;
    }

    // If we get here, we cannot make any more progress.
    status_->BuildFinished();
    if (failures_allowed == 0) {
      if (config_.failures_allowed > 1)
        *err = "subcommands failed";
      else
        *err = "subcommand failed";
    } else if (failures_allowed < config_.failures_allowed)
      *err = "cannot make progress due to previous errors";
    else
      *err = "stuck [this is a bug]";

    return false;
  }

  status_->BuildFinished();
  return true;
}

bool Builder::StartEdge(Edge* edge, string* err) {
  if (edge->is_phony())
    return true;

  status_->BuildEdgeStarted(edge);

  // Create directories necessary for outputs.
  // XXX: this will block; do we care?
  for (vector<Node*>::iterator i = edge->outputs_.begin();
       i != edge->outputs_.end(); ++i) {
    if (!disk_interface_->MakeDirs((*i)->path()))
      return false;
  }

  // Create response file, if needed
  // XXX: this may also block; do we care?
  if (edge->HasRspFile()) {
    if (!disk_interface_->WriteFile(edge->GetRspFile(), edge->GetRspFileContent()))
      return false;
  }

  // start command computing and run it
  if (!command_runner_->StartCommand(edge)) {
    err->assign("command '" + edge->EvaluateCommand() + "' failed.");
    return false;
  }

  return true;
}

void Builder::FinishEdge(Edge* edge, bool success, const string& output) {
  TimeStamp restat_mtime = 0;

  if (success) {
    if (edge->rule().restat() && !config_.dry_run) {
      bool node_cleaned = false;

      for (vector<Node*>::iterator i = edge->outputs_.begin();
           i != edge->outputs_.end(); ++i) {
        TimeStamp new_mtime = disk_interface_->Stat((*i)->path());
        if ((*i)->mtime() == new_mtime) {
          // The rule command did not change the output.  Propagate the clean
          // state through the build graph.
          // Note that this also applies to nonexistent outputs (mtime == 0).
          plan_.CleanNode(log_, *i);
          node_cleaned = true;
        }
      }

      if (node_cleaned) {
        // If any output was cleaned, find the most recent mtime of any
        // (existing) non-order-only input or the depfile.
        for (vector<Node*>::iterator i = edge->inputs_.begin();
             i != edge->inputs_.end() - edge->order_only_deps_; ++i) {
          TimeStamp input_mtime = disk_interface_->Stat((*i)->path());
          if (input_mtime > restat_mtime)
            restat_mtime = input_mtime;
        }

        if (restat_mtime != 0 && !edge->rule().depfile().empty()) {
          TimeStamp depfile_mtime = disk_interface_->Stat(edge->EvaluateDepFile());
          if (depfile_mtime > restat_mtime)
            restat_mtime = depfile_mtime;
        }

        // The total number of edges in the plan may have changed as a result
        // of a restat.
        status_->PlanHasTotalEdges(plan_.command_edge_count());
      }
    }

    // delete the response file on success (if exists)
    if (edge->HasRspFile())
      disk_interface_->RemoveFile(edge->GetRspFile());

    plan_.EdgeFinished(edge);
  }

  if (edge->is_phony())
    return;

  int start_time, end_time;
  status_->BuildEdgeFinished(edge, success, output, &start_time, &end_time);
  if (success && log_)
    log_->RecordCommand(edge, start_time, end_time, restat_mtime);
}
