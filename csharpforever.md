# Java toString issues #



**http://stackoverflow.com/questions/8481829/java-override-tostring**

Actually, I was trying to achieve the same thing, where super.toString() wasn't exactly what I wanted. I believe that this is not possible.

Specifically, I have a domain class in Groovy/Grails, and I wanted to override its toString() method to add some extra information:

class Child extends Parent {

> public String toString() {
> > return super.toString(this) + extra\_info // not possible!!!

> }
}
This is not identical to return super.toString() + extra\_info. In one case I get e.g.

com.example.domain.Parent : 15, extra\_info. (Wrong. The instance is the same, but toString includes type information.)

In the other:

com.example.domain.Child : 15, extra\_info. (Correct, but not possible.)

# No Operator Overload(java) #

yes; it can be abused; (see neuron and brain implementation in C#; and test application) [test form](https://sourceforge.net/p/xperdex/mercurial/ci/default/tree/games/automaton/xperdex.brain.test/Form1.cs); okay that's not a very clear example.  Use operators +,&,|,^, ... to build relations between neurons with a result neuron being the result of the expression... can be complex and result in a network of several neurons....

It can be useful ( see fraction implementation in C#)
[Fraction implementation](https://sourceforge.net/p/xperdex/mercurial/ci/default/tree/xperdex.classes.2/Fraction.cs)
# C++ #
additional Runtime is usually not available easily; STL was never reliable between different distributions.

There were many times that project A would not build on systems A, B, and C with compilers A and B; didn't seem like a good idea to do things with such potential of having to have different code for the same function.

# Code evolution in C# #
```
			if( current_profession.payon[roll] )
				currentPlayer.Cash += current_profession.pay;
```
This code was implemented with Cash as an integer.  But now when cash is set, I want to automatically update text fields that reference cash.  So in C# I just promote the declaration....

```
class Player 
{
   int Cash;
   /*...*/
}

class Player
{
   int cash; // internal;
   public int Cash{
      get
      {
          return cash;
      }
      set
      {
          cash = value;
          UpdateText();
      }
  }
  UpdateText()
  {
      /* for all text fields, set new value */
  }
}
```

and none of the other code that ever referenced cash has to change (has to recompile, but not change)

# Eclipse shortcomings #
Ctrl-tab doesn't work.