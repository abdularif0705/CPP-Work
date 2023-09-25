#include <cstddef>    // for std::size_t
#include <iostream>   // for std::ostream
#include <memory>     // for std::shared_ptr, std::make_shared
#include <ostream>    // for std::ostream
#include <string>     // for std::string
#include <utility>    // for std::move, std::forward
#include <vector>     // for std::vector

// xml_node_base is the base class all XML node types are derived from
class xml_node_base {
public:
  // Virtual destructor to ensure proper cleanup of derived classes
  virtual ~xml_node_base() = default;

// output() is a non-virtual const function that always calls the do_output_*() functions with the stream passed to it.
//    There is no need to make output() virtual as these functions will always be called.
//    By defining output(), this simplifies logic relating to producing output. (XML document nodes have open and close
//    elements (tags) as well as data between such. If output() was not defined, one would have to manually call these
//    three functions --which would be a source of programming issues/mistakes/etc. By defining output() such issues
//    are completely avoided since the logic of output is encapsulated (hidden) appropriately with the derived classes
//    (due to protected scope).
void output(std::ostream& os) const // Outputs the node to an ostream by calling the three protected virtual functions to output the opening tag, the body, and the closing tag of the node
    {
    // do_output_open() and do_output_close() are defined to be pure virtual (abstract) because each element has its own
    // name (and potentially XML attributes that are not dealt with in this assignment) so derived classes need to define such.
    do_output_open(os);
    // do_output_body() is not abstract since it is appropriate to have nodes with no data between the opening and
    // closing elements (tags).
    do_output_body(os);
    do_output_close(os);
  }

protected:
  // Outputs the opening tag of the node
  virtual void do_output_open(std::ostream& os) const = 0;
  // Outputs the body of the node (empty by default)
  virtual void do_output_body(std::ostream&) const { }
  // Outputs the closing tag of the node
  virtual void do_output_close(std::ostream& os) const = 0;
};
// Since OOP requires the use of pointers, the xml_node_base_t type alias is defined to be std::shared_ptr<xml_node_base>
// so that all pointers are managed (and reference counted --although that is not exploited in this assignment).
using xml_node_base_t = std::shared_ptr<xml_node_base>; // A shared pointer to a base XML node
// Since XML nodes can have children, xml_node_bases_t is defined to be a std::vector<xml_node_base_t>
// to make it easier to deal with children.
using xml_node_bases_t = std::vector<xml_node_base_t>; // A vector of shared pointers to base XML nodes


// Template function to create an XML node and return a shared pointer to it
template <typename T, typename... Args>
// make_xml_node<T>(args...) is defined so that T is a type and args... are the arguments used to construct an instance
// of that type. This is done to make it easier to create instances of various XML nodes since traditional/general OOP
// requires instances to be pointers. (This is not exploited in this assignment --but could use for future coding efforts.)
inline xml_node_base_t make_xml_node(Args&&... args) {
  return std::make_shared<T>(std::forward<Args>(args)...);
}

// A basic implementation of an XML node with no child nodes
class xml_node: public virtual xml_node_base { // A class for representing a basic XML node
private:
  std::string name; // the name of the node

  // The xml_node class must inherit virtually and publicly from xml_node_base.
  //    ASIDE: You want only ONE instance of xml_node_base, so you need to use virtual here. To keep xml_node_base's
  //    members' scopes the same, public needs to be used since C++ classes will otherwise inherit privately by default.
public:
  // By writing a constructor or function prototype followed by = delete, you are telling the C++ compiler that
  // such cannot be used, generated, etc. ever. Trying to do so will trigger a compile-time error. The default constructor
  // is being deleted since the user of xml_node must provide a name (i.e., invoke one of the other constructors).
  xml_node() = delete; // Deleted default constructor to ensure that a name is always provided.
  // Constructs a new XML node with the given name.
  //    There is nothing within the constructor body braces --our initialization MUST be before the opening brace of the
  //    constructor body! In the constructor member initialization list, we initialize the private std::string name variable
  //    to the std::string parameter passed to the constructor.
  xml_node(std::string const& name1) : 
  name(name1) {}

protected: // protected scope
  // Override void do_output_open(std::ostream& os) const and:
  //    Output via the std::ostream parameter a '<', followed by the private std::string member, followed by a ,>'.
  //    e.g., "<note>" (with no quotes of course) would be output if the node's name was "note".
  void do_output_open(std::ostream& os) const override { // Outputs the opening tag of the node
    os << '<' << name << '>';
  }
  // Override void do_output_close(std::ostream& os) const
    void do_output_close(std::ostream& os) const override { // Outputs the closing tag of the node
      os << "</" << name << '>';
  }
};

// xml_node defined a node that has no children or data. There are times when a node will have child nodes.
// The xml_node_with_children defines an xml_node with children leveraging inheritance.
// An implementation of an XML node with child nodes
class xml_node_with_children : public xml_node { // The xml_node_with_children class must inherit publicly from the xml_node class. (Do not use virtual.)
// In private scope: Declare a member variable of type xml_node_bases_t to hold the child nodes.
// (This is referred to as the children data member below.)
private:
  xml_node_bases_t children_;

public:
  // Default constructor deleted to force name parameter to be specified
  xml_node_with_children() = delete;
  // after the constructor initialization list, colon first write xml_node{name}  (or xml_node(name)) to "call" the parent
  // class' constructor that accepts a string, and,
  //    ASIDE: This is like using super in Java to invoke a parent class' constructor --except super doesn't exist in C++. (It wouldn't make sense either since C++ supports multiple inheritance. Which class would be called when super was used?)
  // don't worry about initializing the children data member.
  xml_node_with_children(std::string const& name) : xml_node{name} {} // Constructor that takes a name for the node
  //  in the constructor initialization list, invoke the single-parameter std::string constructor in this class by
  //  writing xml_node_with_children{name} (you can use braces or parentheses with constructor syntax)
  //      ASIDE: Again, this is like calling another constructor in the same class in Java albeit with different syntax.
  //      This is sometimes how you can avoid duplicating code when you have a number of constructors.
  xml_node_with_children(std::string const& name, std::size_t reserve) : xml_node_with_children{name} { // Constructor taking the node name and the number of children to reserve space for
    //  in the constructor body, invoke the private children data member's .reserve() function passing in the reserve value to it.
    //      ASIDE: The reserve(n) member function in std::vector and some other types allocates RAM sufficient for n entries
    //      --but it doesn't create elements. This allows the vector to not have to be resized (and copied/moved internally
    //      to do this) unless more than n elements are added to it.
    children_.reserve(reserve);
  }
  // in the constructor initialization list, invoke the parent class' constructor passing the std::string parameter to it,
  // in the constructor initialization list, construct the children private data member by moving the xml_node_bases_t parameter passed to the constructor, and,
  //    i.e., write children_{std::move(children)} if your private data member is called "children_" and your xml_node_bases_t parameter is called "children".
  //    The xml_node_bases_t parameter was passed by value --which makes a copy of the original. For efficiency, you want to move the contents of that copy in to the private data member. If you didn't move it, it would be copied and the first copy would be destroyed. That would be a waste/inefficient.
  //        ASIDE: FYI, the caller of this constructor could have moved data in to this parameter so the "copy" made isn't always an extra copy. If the argument had its data moved into it, then the "copy" isn't really a copy --otherwise it is.
  // the constructor body must be empty.
  // Constructor that takes a name for the node and a vector of child nodes
  xml_node_with_children(std::string const& name, xml_node_bases_t children) : 
    xml_node{name}, 
    children_{std::move(children)} {}

protected:
  // NOTE 1: You cannot call your private data member in this class "children" since this member function uses that name. Rename your private data member if you did call it "children".
  // NOTE 2: children_ in these definitions are what I called my private data member.
  // NOTE 3: Notice that both of the overloads return references --but the const overload returns a const& since the underlying object is const.
  // Defining overload that returns a reference to the vector of child nodes
  auto& children() { return children_; }; // !!!!!!!!!!!!! IMPT: because of return children_ inside of this function auto make the function type std::vector<std::shared_ptr<xml_node_base>> &xml_node_with_children::children()
  // Defining overload that returns a const reference to the vector of child nodes
  auto const& children() const { return children_; }; // const means no touch, it's a memory safety thing in cpp, doesn't change the function at all it just makes it available since the prof likes using const wherever he can

  // In protected scope, override the function void do_output_body(std::ostream& os) const by writing the following code in it:
  //    write a range-for loop to iterate through all children (i.e., the private data member variable) with each element's type being auto const& (you want the const& part; auto is just convenience to avoid writing xml_node_bases_t), and within the range-for body, using -> with the element reference variable to call output(os).
  //        ASIDE: Recall that std::shared_ptr and other smart pointer types overload * and -> so the object can be used like a pointer. Essentially ->output(os) is invoking xml_node_base's output() function.
  // Outputs the body of the node (which consists of the child nodes) to an ostream
  void do_output_body(std::ostream& os) const override {
    for (auto const& child : children_)
      child->output(os);
		}
};
// In many instances one might want a xml_node with a simple datum. value_node<T> defines itself in terms of a xml_node but adds the ability to output its
// stored instance of type T in the body.
template <typename T>
class value_node : public xml_node { // The class must inherit publicly from the xml_node class. (Do not use virtual.)
	// In private scope: Define a data member of type T to hold the datum (which is originally passed to the constructor).
  private:
		T datum;
  // In protected scope: Override void do_output_body(std::ostream& os) const and output the private data member datum to the os stream.
	protected:
		void do_output_body(std::ostream& os) const override {
			os << datum;
		}
	public:
		value_node(std::string const& name, T const& v) : // the parameter v is passed by const reference, meaning it cannot be modified within the function. Therefore, using std::move on v would not provide any benefit, as it is already a const reference and cannot be moved from.
      xml_node{name}, // in the constructor initialization list: pass the std::string name parameter to the parent class constructor,
      // in the constructor initialization list: pass the T parameter to datum (use constructor syntax to do this!), and,
        //    ASIDE: Don't use std::move() here --T was passed as a lvalue. This will always make a copy. (It is possible to have other
        //    constructor overloads that accept rvalue references in order to move when things should be moved so one can have both copying
        //    and moving as is appropriate --but such isn't being done here.)
        // ensure the constructor body is empty.
      datum(v) {} // If v were a movable type and not a const reference, then using std::move could potentially improve performance by avoiding the extra copy, but that is not the case in this particular constructor.
};

// The node class is defined in terms of xml_node_with_children except the number and order of children is fixed. This class ensures that invariant and nicely makes it very easy to create Prof. Q's "note" elements!
//    ASIDE: Unlike HTML, XML does not fix the names of its elements (tags) or underlying structure. This class fixes such to something hard-coded. This is okay
//    in a program: one wants to make it easier to use things --and this class makes it easier to create notes (albeit this assignment only outputs stuff --but
//    you get the idea
// This is a derived class `note` from `xml_node_with_children`.
// This class represents an XML node for a note with four children: to, from, subject, and message.
class note : public xml_node_with_children {
	public:
  // different from a destructor a default deleted constructor is a constructor that is explicitly deleted by the programmer. This means that the compiler will not generate a default constructor
  // for the class. This is useful when we want to prevent the creation of objects without specific arguments or when we don't want to allow copying or moving of objects.
		note() = delete; // default deleted constructor
    // The constructor of this class takes four `std::string` arguments, which are the values to be assigned to the four children nodes.
    // It creates the four child nodes with `make_xml_node` and then pushes them into the `children_` vector of `xml_node_with_children`.
		note(
        std::string const& to,
        std::string const& from,
        std::string const& subject,
        std::string const& message
        ) :
		      xml_node_with_children("note",4) { // in the constructor initialization list, we call the parent class' constructor passing these parameters: "note" and 4,
			this->children().push_back(make_xml_node<value_node<std::string>>("to",to)); // Not needed for this case but using this-> in a constructor can be helpful when there is a local variable or a function parameter with the same name as a class member variable. In such cases, using this-> can disambiguate between the two variables and make it clear that we are referring to the class member variable.
			children().push_back(make_xml_node<value_node<std::string>>("from",from));
			children().push_back(make_xml_node<value_node<std::string>>("subject",subject));
			children().push_back(make_xml_node<value_node<std::string>>("message",message));
		}
};
// As with containers, etc. the actual "document" one wants to hold everything in is special. One should not be permitted to add the document inside of itself
// (at least in this assignment!). To achieve this while exploiting inheritance in order to make it easy to define things, this class does NOT use public
// inheritance, instead it uses protected inheritance.
//    ASIDE: Protected inheritance demotes everything that is public in the parent to protected scope. Everything at protected scope remains protected. By not
//    inheriting publicly, a programmer cannot mistakenly pass instance of the root class to something that accepts xml_node_base*, etc. This is because protected
//    inheritance is not a public "is a" relationship for things outside of the root class.
// This is a derived class `root` from `xml_node_with_children`, which is used as the root of the XML tree.
// The root class must inherit using protected --not public-- access from xml_node_with_children.
class root : protected xml_node_with_children {
	public:
    // This will inherit all of the constructors from the parent class as constructors of this class.
    // e.g. note my_note{"Alice", "Bob", "Meeting", "Let's discuss our project tomorrow!"};
		using xml_node_with_children::xml_node_with_children;
    // This will promote the parent class' output() function (which was defined in xml_node_base) to PUBLIC scope. (The protected inheritance demoted such to
    // protected scope and this line promotes it to public since it is in the public section.)
		using xml_node_with_children::output;
    // This will promote the parent class' children members to public scope. (These were never declared at public scope and now the root class is making them
    // publicly available in this way.)
		using xml_node_with_children::children;
};
// This is an overloaded `<<` operator for the `root` class.
// When this operator is used with an `std::ostream` object and a `root` object,
// it outputs the XML tree rooted at the `root` object using the `output` method.
std::ostream& operator << (std::ostream& os, root const& r){ // the root type must be passed in as const& (you're outputting so treat what you are outputting as read-only!).
	r.output(os);
	return os;
}
int main() {
  root notes{"notes"};
  notes.children().push_back(
    make_xml_node<note>("Joe", "UPS", "Reminder", "Pick up your package!")
  );
  notes.children().push_back(
    make_xml_node<value_node<double>>("priority",54.23)
  );
  std::cout << notes << '\n';
}
