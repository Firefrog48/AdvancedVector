#pragma once

#include <cassert>
#include <cstdlib>
#include <memory>
#include <new>
#include <utility>


template <typename T>
class RawMemory {
public:
    RawMemory() = default;

    explicit RawMemory(size_t capacity)
        : buffer_(Allocate(capacity))
        , capacity_(capacity) {
    }

    RawMemory(const RawMemory& other) = delete;
    RawMemory& operator=(const RawMemory& rhs) = delete;

    RawMemory(RawMemory&& other) noexcept {
        Swap(other);
    }
    
    RawMemory& operator=(RawMemory&& rhs) noexcept {
        if (this != &rhs) {
            Swap(rhs);
        } 
        return *this;
    }

    ~RawMemory() {
        Deallocate(buffer_);
    }

    T* operator+(size_t offset) noexcept {
        // Разрешается получать адрес ячейки памяти, следующей за последним элементом массива
        assert(offset <= capacity_);
        return buffer_ + offset;
    }

    const T* operator+(size_t offset) const noexcept {
        return const_cast<RawMemory&>(*this) + offset;
    }

    const T& operator[](size_t index) const noexcept {
        return const_cast<RawMemory&>(*this)[index];
    }

    T& operator[](size_t index) noexcept {
        assert(index < capacity_);
        return buffer_[index];
    }

    void Swap(RawMemory& other) noexcept {
        std::swap(buffer_, other.buffer_);
        std::swap(capacity_, other.capacity_);
    }

    const T* GetAddress() const noexcept {
        return buffer_;
    }

    T* GetAddress() noexcept {
        return buffer_;
    }

    size_t Capacity() const {
        return capacity_;
    }

private:
    // Выделяет сырую память под n элементов и возвращает указатель на неё
    static T* Allocate(size_t n) {
        return n != 0 ? static_cast<T*>(operator new(n * sizeof(T))) : nullptr;
    }

    // Освобождает сырую память, выделенную ранее по адресу buf при помощи Allocate
    static void Deallocate(T* buf) noexcept {
        operator delete(buf);
    }

    T* buffer_ = nullptr;
    size_t capacity_ = 0;
}; 


template <typename T>
class Vector {
public:

//====================================================CONSTRUCTORS==================================================================
    Vector() = default;
    explicit Vector(size_t size) : data_(size)
                                 , size_(size) 
    {
        std::uninitialized_value_construct_n(this->begin(), size);              
    }

    Vector(const Vector& other) : data_(other.size_) 
                                , size_(other.size_)
    {
        std::uninitialized_copy_n(other.begin(), other.size_, this->begin());
    }

    Vector(Vector&& other) noexcept {
        Swap(other);
    }
   
    Vector& operator=(const Vector& rhs) {
        if (this != &rhs) {
            
            if (rhs.size_ > this->data_.Capacity()) {
                Vector rhs_copy(rhs);
                Swap(rhs_copy);
            
            } else {
                std::copy(rhs.begin(),rhs.begin() + std::min(size_, rhs.size_), this->begin());
                if (rhs.size_ < this->size_) {
                    std::destroy_n(this->begin() + rhs.size_, this->size_ - rhs.size_);
                } else {
                    std::uninitialized_copy_n(rhs.begin() + size_, rhs.size_ - size_, this->begin() + size_);
                }
                size_ = rhs.size_;
            }
        }
        return *this;  
    }

    Vector& operator=(Vector&& rhs) noexcept {
        if (this != &rhs) {
            Swap(rhs);
        }
        return *this;
    }
//=========================================SIZECHANGERS================================================================================
    void Reserve(size_t new_capacity) {
        if (size_ >= new_capacity) {
            return;
        }
        RawMemory<T> new_data(new_capacity);
        MoveOrCopy(this->begin(), this->size_, new_data.GetAddress());
        std::destroy_n(this->begin(), size_);
        data_.Swap(new_data);
    }

    void Resize(size_t new_size) {
        if (size_ > new_size) {
            std::destroy_n(this->begin() + new_size, size_ - new_size);
        } else if (size_ == new_size) {
            return;
        } else {
            if (new_size > data_.Capacity()) {
                const size_t new_capacity = std::max(data_.Capacity() * 2, new_size);
                Reserve(new_capacity);
            }
            std::uninitialized_value_construct_n(this->begin() + size_, new_size - size_);
        }
        size_ = new_size;
    }

//==============================================ITERATORS======================================================================

    using iterator = T*;
    using const_iterator = const T*;
    
    iterator begin() noexcept {
        return data_.GetAddress();
    }
    iterator end() noexcept {
        return data_.GetAddress() + size_;
    }
    const_iterator begin() const noexcept {
        return data_.GetAddress();
    }
    const_iterator end() const noexcept {
        return data_.GetAddress() + size_;
    }
    const_iterator cbegin() const noexcept {
        return data_.GetAddress();
    }
    const_iterator cend() const noexcept {
        return data_.GetAddress() + size_;
    }

    const T& operator[](size_t index) const noexcept {
        return const_cast<Vector&>(*this)[index];
    }

    T& operator[](size_t index) noexcept {
        assert(index < size_);
        return data_[index];
    }

//===========================================ADDING AND DELETING VALUE FUNCTIONS=======================================================
    
    template <typename... Args>
    iterator Emplace(const_iterator pos, Args&&... args) {
                assert(pos >= begin() && pos <= end());
        const size_t index = (pos - begin());
        if (pos == end()) {
            EmplaceBack(std::forward<Args>(args)...);
            return begin() + index;
        }
       
        if (size_ < data_.Capacity()) {

            T tmp_value(std::forward<Args>(args)...);
            new (this->begin() + size_) T(std::move(*(end() - 1)));
            std::move_backward(begin() + index, end() - 1, end());
             *(begin() + index) = std::forward<T>(tmp_value);
        
        } else {
            RawMemory<T> new_data(size_ == 0 ? 1 : size_ * 2);
            
            new(new_data.GetAddress() + index) T(std::forward<Args>(args)...);

            try { MoveOrCopy(begin(), index, new_data.GetAddress()); }
            catch(const std::exception& e) {
                (new_data.GetAddress() + index)->~T();
                throw;
            }

            try { MoveOrCopy(begin() + index, size_ - index, new_data.GetAddress() + index + 1); }
            catch(const std::exception& e) {
                std::destroy_n(new_data.GetAddress(), index);
                throw;
            }
            
            std::destroy_n(begin(), size_);
            data_.Swap(new_data);
            
        }
        size_++;
        return begin() + index;    
    }

    iterator Erase(const_iterator pos) {
        assert(pos >= begin() && pos <= end());
        const size_t index = pos - begin();
        std::move(begin() + index + 1, end(), begin() + index );
        (end()-1)->~T();
        size_--;
        return begin() + index;
    }

    iterator Insert(const_iterator pos, const T& value) {
        return Emplace(pos, value);
    }

    iterator Insert(const_iterator pos, T&& value) {
        return Emplace(pos, std::move(value));
    }

    void Swap(Vector& other) noexcept {
        data_.Swap(other.data_);
        std::swap(size_, other.size_);
    }

    template <typename Type>
    void PushBack(Type&& value) {
        EmplaceBack(std::forward<Type>(value));
    }

    void PushBack(const T& value) {
        EmplaceBack(value);
    }
    
    template <typename... Args>
    T& EmplaceBack(Args&&... args) {
        if (data_.Capacity() == size_) {
            RawMemory<T> new_data(size_ == 0 ? 1 : size_ * 2);
            new (new_data.GetAddress() + size_) T(std::forward<Args>(args)...);
            if (size_ != 0) { 
                MoveOrCopy(this->begin(), size_, new_data.GetAddress());
                std::destroy_n(this->begin(), size_);
            }
            data_.Swap(new_data);
            
        } else {
            new (this->begin() + size_) T(std::forward<Args>(args)...);
        }
        size_++;
        return data_[size_ - 1];
    }
    
    
    void PopBack() noexcept {
        if (!(Empty())) {
            data_[--size_].~T();
        }
    }
//====================================================INFO FUNCTIONS===================================================================
    size_t Size() const noexcept {
        return size_;
    }

    size_t Capacity() const noexcept {
        return data_.Capacity();
    }

    bool Empty() const noexcept {
        return size_ == 0;
    }

//=====================================================DESTRUCTOR=======================================================================

    ~Vector() {
        std::destroy_n(data_.GetAddress(), size_);
    }

private:


    template <typename It> 
    void MoveOrCopy(It begin, size_t count_value, It result) {
        if constexpr (std::is_nothrow_move_constructible_v<T> || !std::is_copy_constructible_v<T>) {
            std::uninitialized_move_n(begin, count_value, result);
        } else {
            std::uninitialized_copy_n(begin, count_value, result);
        }
    }

    RawMemory<T> data_;
    size_t size_ = 0;
};


