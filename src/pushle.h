#pragma once

#include <cassert>
#include <cstdint>
#include <cstddef>
#include <string>

#include "ops.h"

#include <fmt/core.h>

#ifndef VM_DEBUG_LEVEL
#define VM_DEBUG_LEVEL 0
#endif

#if VM_DEBUG_LEVEL >= 1
  #define VM_DEBUG_1(...) fmt::print("[vm]\t {}\n", fmt::format(__VA_ARGS__))
#else
  #define VM_DEBUG_1(...)
#endif

#if VM_DEBUG_LEVEL >= 2
  #define VM_DEBUG_2(...) fmt::print("[vm]\t {}\n", fmt::format(__VA_ARGS__))
#else
  #define VM_DEBUG_2(...)
#endif


#define _FN_T(return_type, prefix, ...) \
  return_type prefix##i8(__VA_ARGS__); \
  return_type prefix##u8(__VA_ARGS__); \
  return_type prefix##bool(__VA_ARGS__); \
  return_type prefix##i16(__VA_ARGS__); \
  return_type prefix##u16(__VA_ARGS__); \
  return_type prefix##i32(__VA_ARGS__); \
  return_type prefix##u32(__VA_ARGS__); \
  return_type prefix##f32(__VA_ARGS__); \
  return_type prefix##i64(__VA_ARGS__); \
  return_type prefix##u64(__VA_ARGS__); \
  return_type prefix##f64(__VA_ARGS__);

#define _FN_T_T(return_type, prefix, ...) \
  return_type prefix##i8(int8_t __VA_ARGS__); \
  return_type prefix##u8(uint8_t __VA_ARGS__); \
  return_type prefix##bool(bool __VA_ARGS__); \
  return_type prefix##i16(int16_t __VA_ARGS__); \
  return_type prefix##u16(uint16_t __VA_ARGS__); \
  return_type prefix##i32(int32_t __VA_ARGS__); \
  return_type prefix##u32(uint32_t __VA_ARGS__); \
  return_type prefix##f32(float __VA_ARGS__); \
  return_type prefix##i64(int64_t __VA_ARGS__); \
  return_type prefix##u64(uint64_t __VA_ARGS__); \
  return_type prefix##f64(double __VA_ARGS__);


#define _FN_N(return_type, prefix, ...) \
  return_type prefix##i8(__VA_ARGS__); \
  return_type prefix##u8(__VA_ARGS__); \
  return_type prefix##i16(__VA_ARGS__); \
  return_type prefix##u16(__VA_ARGS__); \
  return_type prefix##i32(__VA_ARGS__); \
  return_type prefix##u32(__VA_ARGS__); \
  return_type prefix##f32(__VA_ARGS__); \
  return_type prefix##i64(__VA_ARGS__); \
  return_type prefix##u64(__VA_ARGS__); \
  return_type prefix##f64(__VA_ARGS__); \

#define _FN_I(return_type, prefix, ...) \
  return_type prefix##i8(__VA_ARGS__); \
  return_type prefix##i16(__VA_ARGS__); \
  return_type prefix##i32(__VA_ARGS__); \
  return_type prefix##f32(__VA_ARGS__); \
  return_type prefix##i64(__VA_ARGS__); \
  return_type prefix##f64(__VA_ARGS__);

#define _FN_S(return_type, prefix, ...) \
  return_type prefix##1(__VA_ARGS__); \
  return_type prefix##2(__VA_ARGS__); \
  return_type prefix##4(__VA_ARGS__); \
  return_type prefix##8(__VA_ARGS__);

namespace pushle {
  const size_t VM_STACK_SIZE = 1024 * 1024;
  const size_t VM_CALL_STACK_SIZE = 1024;
  const size_t VM_SCOPE_LOCALS_SIZE = 0xff;

  class Value {
  public:
    Value() : _type(_none) {}

    Value(int8_t v)      : _type(_i8),   value({ ._i8 = v }) {}
    Value(uint8_t v)     : _type(_u8),   value({ ._u8 = v }) {}
    Value(bool v)        : _type(_bool), value({ ._bool = v }) {}
    Value(int16_t v)     : _type(_i16),  value({ ._i16 = v }) {}
    Value(uint16_t v)    : _type(_u16),  value({ ._u16 = v }) {}
    Value(int32_t v)     : _type(_i32),  value({ ._i32 = v }) {}
    Value(uint32_t v)    : _type(_u32),  value({ ._u32 = v }) {}
    Value(float v)       : _type(_f32),  value({ ._f32 = v }) {}
    Value(int64_t v)     : _type(_i64),  value({ ._i64 = v }) {}
    Value(uint64_t v)    : _type(_u64),  value({ ._u64 = v }) {}
    Value(double v)      : _type(_f64),  value({ ._f64 = v }) {}

    operator int8_t() const { return value._i8; }
    operator uint8_t() const { return value._u8; }
    operator bool() const { return value._bool; }
    operator int16_t() const { return value._i16; }
    operator uint16_t() const { return value._u16; }
    operator int32_t() const { return value._i32; }
    operator uint32_t() const { return value._u32; }
    operator float() const { return value._f32; }
    operator int64_t() const { return value._i64; }
    operator uint64_t() const { return value._u64; }
    operator double() const { return value._f64; }

    Value& operator=(int8_t v) { _type = _i8; value._i8 = v; return *this; }
    Value& operator=(uint8_t v) { _type = _u8; value._u8 = v; return *this; }
    Value& operator=(bool v) { _type = _bool; value._bool = v; return *this; }
    Value& operator=(int16_t v) { _type = _i16; value._i16 = v; return *this; }
    Value& operator=(uint16_t v) { _type = _u16; value._u16 = v; return *this; }
    Value& operator=(int32_t v) { _type = _i32; value._i32 = v; return *this; }
    Value& operator=(uint32_t v) { _type = _u32; value._u32 = v; return *this; }
    Value& operator=(float v) { _type = _f32; value._f32 = v; return *this; }
    Value& operator=(int64_t v) { _type = _i64; value._i64 = v; return *this; }
    Value& operator=(uint64_t v) { _type = _u64; value._u64 = v; return *this; }
    Value& operator=(double v) { _type = _f64; value._f64 = v; return *this; }

    // ~Value();

    inline DataType get_type() const {
      return _type;
    }

    inline std::string get_type_name() const {
      switch (_type) {
        case _none: return "none";
        case _i8: return "i8";
        case _u8: return "u8";
        case _bool: return "bool";
        case _i16: return "i16";
        case _u16: return "u16";
        case _i32: return "i32";
        case _u32: return "u32";
        case _f32: return "f32";
        case _i64: return "i64";
        case _u64: return "u64";
        case _f64: return "f64";
        default: return "UNKNOWN";
      }
    }

    inline std::string get_value_string() const {
      switch (_type) {
        case _none: return "none";
        case _i8: return std::to_string(value._i8);
        case _u8: return std::to_string(value._u8);
        case _bool: return std::to_string(value._bool);
        case _i16: return std::to_string(value._i16);
        case _u16: return std::to_string(value._u16);
        case _i32: return std::to_string(value._i32);
        case _u32: return std::to_string(value._u32);
        case _f32: return std::to_string(value._f32);
        case _i64: return std::to_string(value._i64);
        case _u64: return std::to_string(value._u64);
        case _f64: return std::to_string(value._f64);
        default: return "UNKNOWN";
      }
    }

    inline size_t size() const {
      switch (_type) {
        case _i8:
        case _u8:
        case _bool:
          return 1;
        case _i16:
        case _u16:
          return 2;
        case _i32:
        case _u32:
        case _f32:
          return 4;
        case _i64:
        case _u64:
        case _f64:
          return 8;
          return 16;
        default:
          return 0;
      }
    }
    
    inline int8_t as_i8_safe() const {
      assert(_type == _i8);
      return value._i8;
    }

    inline uint8_t as_u8_safe() const {
      assert(_type == _u8);
      return value._u8;
    }

    inline bool as_bool_safe() const {
      assert(_type == _bool);
      return value._bool;
    }

    inline int16_t as_i16_safe() const {
      assert(_type == _i16);
      return value._i16;
    }

    inline uint16_t as_u16_safe() const {
      assert(_type == _u16);
      return value._u16;
    }

    inline int32_t as_i32_safe() const {
      assert(_type == _i32);
      return value._i32;
    }

    inline uint32_t as_u32_safe() const {
      assert(_type == _u32);
      return value._u32;
    }

    inline float as_f32_safe() const {
      assert(_type == _f32);
      return value._f32;
    }

    inline int64_t as_i64_safe() const {
      assert(_type == _i64);
      return value._i64;
    }

    inline uint64_t as_u64_safe() const {
      assert(_type == _u64);
      return value._u64;
    }

    inline double as_f64_safe() const {
      assert(_type == _f64);
      return value._f64;
    }


  private:
    DataType _type;
    union {
      int8_t _i8;
      uint8_t _u8;
      bool _bool;
      int16_t _i16;
      uint16_t _u16;
      int32_t _i32;
      uint32_t _u32;
      float _f32;
      int64_t _i64;
      uint64_t _u64;
      double _f64;
    } value;
  };

  class VMScope {
  public:
    VMScope() = default;
    ~VMScope() = default;

    inline Value *local(size_t index) {
      assert(index < VM_SCOPE_LOCALS_SIZE);
      return &locals[index];
    }

    inline void local(size_t index, Value value) {
      assert(index < VM_SCOPE_LOCALS_SIZE);
      locals[index] = value;
    }
  private:
    Value locals[VM_SCOPE_LOCALS_SIZE];
  };

  class VM {
  public:
    VM();
    void run(const uint8_t *program, size_t size);
    inline int8_t get_i8() { return *(int8_t *)ref(sizeof(int8_t)); }
    inline uint8_t get_u8() { return *(uint8_t *)ref(sizeof(uint8_t)); }
    inline bool get_bool() { return *(bool *)ref(sizeof(bool)); }
    inline int16_t get_i16() { return *(int16_t *)ref(sizeof(int16_t)); }
    inline uint16_t get_u16() { return *(uint16_t *)ref(sizeof(uint16_t)); }
    inline int32_t get_i32() { return *(int32_t *)ref(sizeof(int32_t)); }
    inline uint32_t get_u32() { return *(uint32_t *)ref(sizeof(uint32_t)); }
    inline float get_f32() { return *(float *)ref(sizeof(float)); }
    inline int64_t get_i64() { return *(int64_t *)ref(sizeof(int64_t)); }
    inline uint64_t get_u64() { return *(uint64_t *)ref(sizeof(uint64_t)); }
    inline double get_f64() { return *(double *)ref(sizeof(double)); }

  private:
    uint8_t stack[VM_STACK_SIZE];
    uint8_t *stack_top;

    const uint8_t *program;
    size_t program_size;
    const uint8_t *instruction;
    // uint8_t *call_stack[VM_CALL_STACK_SIZE];
    // uint8_t **call_stack_top;
    // uint8_t **call_stack_next;

    VMScope scope; // TODO: make this a stack

    int8_t reg_cmp;
    int8_t reg_err;
    void *reg_ret; // TODO
    // TODO: heap

    void *read(size_t size);
    bool step(); // returns false if VM is finished
    void push(void *value, size_t size);
    void *pop(size_t size);
    void *ref(size_t offset);

    _FN_T_T(void, push_, value)
    _FN_T(void, popl_, VMScope *scope, uint8_t index)
    _FN_T(void, pushl_, VMScope *scope, uint8_t index)
    _FN_T_T(void, setl_, value, VMScope *scope, uint8_t index)
    _FN_N(void, add_)
    _FN_N(void, sub_)
    _FN_N(void, mul_)
    _FN_N(void, div_)
    _FN_N(void, rem_)
    _FN_I(void, abs_)

    _FN_N(void, dec_)
    _FN_N(void, inc_)

    void dupg(uint8_t n);
    _FN_S(void, dup)
    void swapg(uint8_t n);
    _FN_S(void, swap)
    void popg(uint8_t n);
    _FN_S(void, pop)
    _FN_N(void, cmp_)
    void jz(size_t offset);
    void jnz(size_t offset);
    void jl(size_t offset);
    void jg(size_t offset);
    void jnl(size_t offset);
    void jng(size_t offset);
    void jmp(size_t offset);
    void ret();
    void dbg(int8_t i);
    void sig(int8_t signal);
  };
};
