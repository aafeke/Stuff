#include <utility>
#include <iostream>

template <typename T>
class uptr {
    T* ptr_;

public:
    uptr(T* ptr) : ptr_(ptr) {}
    
    ~uptr() { delete ptr_; }
    
    uptr(const uptr<T>&) = delete; // No copy.
    
    uptr<T>& operator=(const uptr<T>&) = delete; // No copy fr.

    uptr(uptr<T>&& other) {
        std::cout << "Move constructor got called\n";
        ptr_ = other.ptr_;
        other.ptr_ = nullptr; // prevent double free
    }

    uptr<T>& operator=(uptr<T>&& rhs) {
        std::cout << "Move assignment operator got called\n";
        if (this != &rhs) {
            ptr_ = rhs.ptr_;
            rhs.ptr_ = nullptr; // prevent double free
        }
        return *this;
    }

    T* operator->() { return ptr_; }

    explicit operator bool() const { return ptr_; }
};

template <typename T>
uptr<T> make_unique(T* ptr) {
    return uptr<T>(ptr);
}

template <typename T, typename... Args>
uptr<T> make_unique(Args&&... args) {
    return uptr<T>(new T(std::forward<Args>(args)...));
}

// ###############################################################
// ###############################################################
// ###############################################################

class Object {
public:
    int member_;
    Object(int num): member_(num) {
        std::cout << "Object constructor got called. member_ = " 
            << member_ <<  "\n";
    }

    ~Object() {
        std::cout << "Object destructor got called. member_ = " 
            << member_ <<  "\n";
    }
};

int main() {
    std::cout << "Beginning of the inner scope.\n";

    {
        auto ptr  = make_unique<Object>(new Object(3));
        auto ptr2 = make_unique<Object>(5);
        auto ptr3 = std::move(ptr2);    // move constructor
        uptr<Object> ptr4(0);
        ptr4 = std::move(ptr3);         // move assignment operator.
                                        // the initial object ptr4 held leaked.

        std::cout << "Access test: ptr->member_  = " << ptr->member_ << "\n";
        // std::cout << "Access test: ptr->member_  = " << ptr2->member_ << "\n";  // Dereferencing nullptr.
        // std::cout << "Access test: ptr->member_  = " << ptr3->member_ << "\n";  // Dereferencing nullptr.
        std::cout << "Access test: ptr2->member_ = " << ptr4->member_ << "\n";
    }
       
    std::cout << "End of the inner scope.\n";
    return 0;
}
