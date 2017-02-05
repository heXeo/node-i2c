#include <nan.h>
#include <errno.h>
#include <string.h>
#include "i2c.h"

#define ASSERT_ARGUMENTS_LENGTH(X) \
  if (info.Length() != X) { \
    return Nan::ThrowRangeError("Function expects " #X " arguments"); \
  }

#define ASSERT_ARGUMENT_TYPE(ID, TYPE) \
  if (!info[ID]->Is##TYPE()) { \
    return Nan::ThrowTypeError("Argument " #ID " must be " #TYPE); \
  }

#define ASSERT_NEW_CALL() \
  if (!info.IsConstructCall()) { \
    return Nan::ThrowError("Cannot call constructor as function, you " \
                            "need to use 'new' keyword"); \
  }

#define RETURN(VALUE) info.GetReturnValue().Set(VALUE)

#define GET(X, TYPE) info[X]->TYPE##Value()

#define THROW_ERRNO() Nan::ThrowError(strerror(errno))

class Device : public Nan::ObjectWrap {
  public:
    static NAN_MODULE_INIT(Init);

  private:
    explicit Device(const std::string& device, uint8_t address);
    ~Device(void);

    static NAN_METHOD(New);

    static NAN_METHOD(read);
    static NAN_METHOD(readReg8);
    static NAN_METHOD(readReg16);
    static NAN_METHOD(write);
    static NAN_METHOD(writeReg8);
    static NAN_METHOD(writeReg16);

    static inline Nan::Persistent<v8::Function>& constructor() {
      static Nan::Persistent<v8::Function> my_constructor;
      return my_constructor;
    }

    int32_t fd;
};

Device::Device(const std::string& device, uint8_t address) {
  if ((this->fd = i2c_open(device.c_str(), address)) < 0) {
    THROW_ERRNO();
  }
}

Device::~Device(void) {
  i2c_close(this->fd);
}

NAN_MODULE_INIT(Device::Init) {
  v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(New);
  tpl->SetClassName(Nan::New("Device").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  SetPrototypeMethod(tpl, "read", read);
  SetPrototypeMethod(tpl, "readReg8", readReg8);
  SetPrototypeMethod(tpl, "readReg16", readReg16);
  SetPrototypeMethod(tpl, "write", read);
  SetPrototypeMethod(tpl, "writeReg8", readReg8);
  SetPrototypeMethod(tpl, "writeReg16", readReg16);

  constructor().Reset(Nan::GetFunction(tpl).ToLocalChecked());
  Nan::Set(target, Nan::New("Device").ToLocalChecked(),
    Nan::GetFunction(tpl).ToLocalChecked());
}

NAN_METHOD(Device::New) {
  ASSERT_NEW_CALL();
  ASSERT_ARGUMENTS_LENGTH(2);
  ASSERT_ARGUMENT_TYPE(0, String);
  ASSERT_ARGUMENT_TYPE(1, Uint32);

  Nan::Utf8String device(Nan::To<v8::Object>(info[0]).ToLocalChecked());
  uint8_t address = GET(1, Uint32);

  Device* obj = new Device(*device, address);

  obj->Wrap(info.This());

  RETURN(info.This());
}

NAN_METHOD(Device::read) {
  ASSERT_ARGUMENTS_LENGTH(0);

  uint8_t out;
  Device* device = Nan::ObjectWrap::Unwrap<Device>(info.Holder());

  if (i2c_read(device->fd, &out) < 0) {
    return THROW_ERRNO();
  }

  RETURN(Nan::New(out));
}

NAN_METHOD(Device::readReg8) {
  ASSERT_ARGUMENTS_LENGTH(1);
  ASSERT_ARGUMENT_TYPE(0, Uint32);

  uint8_t out;
  uint8_t reg = GET(0, Uint32);
  Device* device = Nan::ObjectWrap::Unwrap<Device>(info.Holder());

  if (i2c_read_reg8(device->fd, reg, &out) < 0) {
    return THROW_ERRNO();
  }

  RETURN(Nan::New(out));
}

NAN_METHOD(Device::readReg16) {
  ASSERT_ARGUMENTS_LENGTH(1);
  ASSERT_ARGUMENT_TYPE(0, Uint32);

  uint16_t out;
  uint8_t reg = GET(0, Uint32);
  Device* device = Nan::ObjectWrap::Unwrap<Device>(info.Holder());

  if (i2c_read_reg16(device->fd, reg, &out) < 0) {
    return THROW_ERRNO();
  }

  RETURN(Nan::New(out));
}

NAN_METHOD(Device::write) {
  ASSERT_ARGUMENTS_LENGTH(1);
  ASSERT_ARGUMENT_TYPE(1, Uint32);

  uint8_t in = GET(0, Uint32);
  Device* device = Nan::ObjectWrap::Unwrap<Device>(info.Holder());

  if (i2c_write(device->fd, in) < 0) {
    return THROW_ERRNO();
  }

  RETURN(Nan::Undefined());
}

NAN_METHOD(Device::writeReg8) {
  ASSERT_ARGUMENTS_LENGTH(2);
  ASSERT_ARGUMENT_TYPE(0, Uint32);
  ASSERT_ARGUMENT_TYPE(1, Uint32);

  uint8_t reg = GET(0, Uint32);
  uint8_t in = GET(0, Uint32);
  Device* device = Nan::ObjectWrap::Unwrap<Device>(info.Holder());

  if (i2c_write_reg8(device->fd, reg, in) < 0) {
    return THROW_ERRNO();
  }

  RETURN(Nan::Undefined());
}

NAN_METHOD(Device::writeReg16) {
  ASSERT_ARGUMENTS_LENGTH(2);
  ASSERT_ARGUMENT_TYPE(0, Uint32);
  ASSERT_ARGUMENT_TYPE(1, Uint32);

  uint8_t reg = GET(0, Uint32);
  uint16_t in = GET(0, Uint32);
  Device* device = Nan::ObjectWrap::Unwrap<Device>(info.Holder());

  if (i2c_write_reg16(device->fd, reg, in) < 0) {
    return THROW_ERRNO();
  }

  RETURN(Nan::Undefined());
}

NODE_MODULE(i2c, Device::Init);
