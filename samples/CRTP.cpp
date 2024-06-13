#include <iostream>
#include <type_traits>

template <typename T> class Base
{
    template <typename U, typename = void> struct void_hello_sequence : std::false_type
    {
    };
    template <typename U>
    struct void_hello_sequence<U, std::enable_if_t<std::is_same_v<decltype(std::declval<U>().hello()), void>>>
        : std::true_type
    {
    };

  public:
    virtual ~Base()
    {
        std::cout << "Base::~Base" << std::endl;
        // delete static_cast<T *>(this);
    }
    Base()
    {
        std::cout << "Base::Base" << std::endl;
        static_assert(void_hello_sequence<T>::value, "T should have `void hello()` method");
    }
};

class Derived : public Base<Derived>
{
  public:
    virtual ~Derived()
    {
        std::cout << "Derived::~Derived" << std::endl;
    }
    void hello()
    {
        std::cout << "CRTP Derived::hello" << std::endl;
    }
    Derived()
    {
        std::cout << "Derived::Derived" << std::endl;
    }
};
class DerivedDerived : public Derived
{
  public:
    DerivedDerived()
    {
        std::cout << "DerivedDerived::DerivedDerived" << std::endl;
    }
    ~DerivedDerived()
    {
        std::cout << "DerivedDerived::~DerivedDerived" << std::endl;
    }
};

class NormalBase
{
  public:
    ~NormalBase()
    {
        std::cout << "NormalBase::~NormalBase" << std::endl;
    }
    void hello()
    {
        std::cout << "NormalBase::hello" << std::endl;
    }
};

class NormalDerived : public NormalBase
{
  public:
    ~NormalDerived()
    {
        std::cout << "NormalDerived::~NormalDerived" << std::endl;
    }
    void hello()
    {
        std::cout << "NormalDerived::hello" << std::endl;
    }
};

int main()
{
    Base<Derived> *b = new DerivedDerived();
    static_cast<DerivedDerived *>(b)->hello();
    delete b;

    NormalBase *nb = new NormalDerived();
    nb->hello();
    delete nb;

    NormalBase *nb2 = new NormalDerived();
    static_cast<NormalDerived *>(nb2)->hello();
    delete nb2;
    return 0;
}