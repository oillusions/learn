#pragma once

template<typename T>
class RAIIWrapper {
    public:
        RAIIWrapper() = default;
        virtual ~RAIIWrapper() = default;
        RAIIWrapper(const RAIIWrapper&) = delete;
        RAIIWrapper(RAIIWrapper&&) = default;
        RAIIWrapper& operator = (const RAIIWrapper&) = delete;
        RAIIWrapper& operator = (RAIIWrapper&&) = default;

        T& get() {
            return _value;
        }

        const T& get() const {
            return _value;
        }

        operator T& () {
            return _value;
        }

        operator const T& () const {
            return _value;
        }

        T* operator & () {
            return &_value;
        }

        T* operator -> () {
            return &_value;
        }
    protected:
        T _value;
};
