  template <>
  void kernel_t<Serial>::operator () (const kernelArg &arg0){
    SerialKernelData_t &data_ = *((SerialKernelData_t*) data);
    handleFunction_t tmpKernel = (handleFunction_t) data_.handle;
    int occaKernelArgs[6];

    occaKernelArgs[0] = outer.z;
    occaKernelArgs[1] = outer.y;
    occaKernelArgs[2] = outer.x;
    occaKernelArgs[3] = inner.z;
    occaKernelArgs[4] = inner.y;
    occaKernelArgs[5] = inner.x;

    int occaInnerId0 = 0, occaInnerId1 = 0, occaInnerId2 = 0;

    tmpKernel(occaKernelArgs,
              occaInnerId0, occaInnerId1, occaInnerId2,
              arg0.data());
  }

  template <>
  void kernel_t<Serial>::operator () (const kernelArg &arg0,  const kernelArg &arg1){
    SerialKernelData_t &data_ = *((SerialKernelData_t*) data);
    handleFunction_t tmpKernel = (handleFunction_t) data_.handle;
    int occaKernelArgs[6];

    occaKernelArgs[0] = outer.z;
    occaKernelArgs[1] = outer.y;
    occaKernelArgs[2] = outer.x;
    occaKernelArgs[3] = inner.z;
    occaKernelArgs[4] = inner.y;
    occaKernelArgs[5] = inner.x;

    int occaInnerId0 = 0, occaInnerId1 = 0, occaInnerId2 = 0;

    tmpKernel(occaKernelArgs,
              occaInnerId0, occaInnerId1, occaInnerId2,
              arg0.data(),
              arg1.data());
  }

  template <>
  void kernel_t<Serial>::operator () (const kernelArg &arg0,  const kernelArg &arg1,  const kernelArg &arg2){
    SerialKernelData_t &data_ = *((SerialKernelData_t*) data);
    handleFunction_t tmpKernel = (handleFunction_t) data_.handle;
    int occaKernelArgs[6];

    occaKernelArgs[0] = outer.z;
    occaKernelArgs[1] = outer.y;
    occaKernelArgs[2] = outer.x;
    occaKernelArgs[3] = inner.z;
    occaKernelArgs[4] = inner.y;
    occaKernelArgs[5] = inner.x;

    int occaInnerId0 = 0, occaInnerId1 = 0, occaInnerId2 = 0;

    tmpKernel(occaKernelArgs,
              occaInnerId0, occaInnerId1, occaInnerId2,
              arg0.data(),
              arg1.data(),
              arg2.data());
  }

  template <>
  void kernel_t<Serial>::operator () (const kernelArg &arg0,  const kernelArg &arg1,  const kernelArg &arg2, 
                      const kernelArg &arg3){
    SerialKernelData_t &data_ = *((SerialKernelData_t*) data);
    handleFunction_t tmpKernel = (handleFunction_t) data_.handle;
    int occaKernelArgs[6];

    occaKernelArgs[0] = outer.z;
    occaKernelArgs[1] = outer.y;
    occaKernelArgs[2] = outer.x;
    occaKernelArgs[3] = inner.z;
    occaKernelArgs[4] = inner.y;
    occaKernelArgs[5] = inner.x;

    int occaInnerId0 = 0, occaInnerId1 = 0, occaInnerId2 = 0;

    tmpKernel(occaKernelArgs,
              occaInnerId0, occaInnerId1, occaInnerId2,
              arg0.data(),
              arg1.data(),
              arg2.data(),
              arg3.data());
  }

  template <>
  void kernel_t<Serial>::operator () (const kernelArg &arg0,  const kernelArg &arg1,  const kernelArg &arg2, 
                      const kernelArg &arg3,  const kernelArg &arg4){
    SerialKernelData_t &data_ = *((SerialKernelData_t*) data);
    handleFunction_t tmpKernel = (handleFunction_t) data_.handle;
    int occaKernelArgs[6];

    occaKernelArgs[0] = outer.z;
    occaKernelArgs[1] = outer.y;
    occaKernelArgs[2] = outer.x;
    occaKernelArgs[3] = inner.z;
    occaKernelArgs[4] = inner.y;
    occaKernelArgs[5] = inner.x;

    int occaInnerId0 = 0, occaInnerId1 = 0, occaInnerId2 = 0;

    tmpKernel(occaKernelArgs,
              occaInnerId0, occaInnerId1, occaInnerId2,
              arg0.data(),
              arg1.data(),
              arg2.data(),
              arg3.data(),
              arg4.data());
  }

  template <>
  void kernel_t<Serial>::operator () (const kernelArg &arg0,  const kernelArg &arg1,  const kernelArg &arg2, 
                      const kernelArg &arg3,  const kernelArg &arg4,  const kernelArg &arg5){
    SerialKernelData_t &data_ = *((SerialKernelData_t*) data);
    handleFunction_t tmpKernel = (handleFunction_t) data_.handle;
    int occaKernelArgs[6];

    occaKernelArgs[0] = outer.z;
    occaKernelArgs[1] = outer.y;
    occaKernelArgs[2] = outer.x;
    occaKernelArgs[3] = inner.z;
    occaKernelArgs[4] = inner.y;
    occaKernelArgs[5] = inner.x;

    int occaInnerId0 = 0, occaInnerId1 = 0, occaInnerId2 = 0;

    tmpKernel(occaKernelArgs,
              occaInnerId0, occaInnerId1, occaInnerId2,
              arg0.data(),
              arg1.data(),
              arg2.data(),
              arg3.data(),
              arg4.data(),
              arg5.data());
  }

  template <>
  void kernel_t<Serial>::operator () (const kernelArg &arg0,  const kernelArg &arg1,  const kernelArg &arg2, 
                      const kernelArg &arg3,  const kernelArg &arg4,  const kernelArg &arg5, 
                      const kernelArg &arg6){
    SerialKernelData_t &data_ = *((SerialKernelData_t*) data);
    handleFunction_t tmpKernel = (handleFunction_t) data_.handle;
    int occaKernelArgs[6];

    occaKernelArgs[0] = outer.z;
    occaKernelArgs[1] = outer.y;
    occaKernelArgs[2] = outer.x;
    occaKernelArgs[3] = inner.z;
    occaKernelArgs[4] = inner.y;
    occaKernelArgs[5] = inner.x;

    int occaInnerId0 = 0, occaInnerId1 = 0, occaInnerId2 = 0;

    tmpKernel(occaKernelArgs,
              occaInnerId0, occaInnerId1, occaInnerId2,
              arg0.data(),
              arg1.data(),
              arg2.data(),
              arg3.data(),
              arg4.data(),
              arg5.data(),
              arg6.data());
  }

  template <>
  void kernel_t<Serial>::operator () (const kernelArg &arg0,  const kernelArg &arg1,  const kernelArg &arg2, 
                      const kernelArg &arg3,  const kernelArg &arg4,  const kernelArg &arg5, 
                      const kernelArg &arg6,  const kernelArg &arg7){
    SerialKernelData_t &data_ = *((SerialKernelData_t*) data);
    handleFunction_t tmpKernel = (handleFunction_t) data_.handle;
    int occaKernelArgs[6];

    occaKernelArgs[0] = outer.z;
    occaKernelArgs[1] = outer.y;
    occaKernelArgs[2] = outer.x;
    occaKernelArgs[3] = inner.z;
    occaKernelArgs[4] = inner.y;
    occaKernelArgs[5] = inner.x;

    int occaInnerId0 = 0, occaInnerId1 = 0, occaInnerId2 = 0;

    tmpKernel(occaKernelArgs,
              occaInnerId0, occaInnerId1, occaInnerId2,
              arg0.data(),
              arg1.data(),
              arg2.data(),
              arg3.data(),
              arg4.data(),
              arg5.data(),
              arg6.data(),
              arg7.data());
  }

  template <>
  void kernel_t<Serial>::operator () (const kernelArg &arg0,  const kernelArg &arg1,  const kernelArg &arg2, 
                      const kernelArg &arg3,  const kernelArg &arg4,  const kernelArg &arg5, 
                      const kernelArg &arg6,  const kernelArg &arg7,  const kernelArg &arg8){
    SerialKernelData_t &data_ = *((SerialKernelData_t*) data);
    handleFunction_t tmpKernel = (handleFunction_t) data_.handle;
    int occaKernelArgs[6];

    occaKernelArgs[0] = outer.z;
    occaKernelArgs[1] = outer.y;
    occaKernelArgs[2] = outer.x;
    occaKernelArgs[3] = inner.z;
    occaKernelArgs[4] = inner.y;
    occaKernelArgs[5] = inner.x;

    int occaInnerId0 = 0, occaInnerId1 = 0, occaInnerId2 = 0;

    tmpKernel(occaKernelArgs,
              occaInnerId0, occaInnerId1, occaInnerId2,
              arg0.data(),
              arg1.data(),
              arg2.data(),
              arg3.data(),
              arg4.data(),
              arg5.data(),
              arg6.data(),
              arg7.data(),
              arg8.data());
  }

  template <>
  void kernel_t<Serial>::operator () (const kernelArg &arg0,  const kernelArg &arg1,  const kernelArg &arg2, 
                      const kernelArg &arg3,  const kernelArg &arg4,  const kernelArg &arg5, 
                      const kernelArg &arg6,  const kernelArg &arg7,  const kernelArg &arg8, 
                      const kernelArg &arg9){
    SerialKernelData_t &data_ = *((SerialKernelData_t*) data);
    handleFunction_t tmpKernel = (handleFunction_t) data_.handle;
    int occaKernelArgs[6];

    occaKernelArgs[0] = outer.z;
    occaKernelArgs[1] = outer.y;
    occaKernelArgs[2] = outer.x;
    occaKernelArgs[3] = inner.z;
    occaKernelArgs[4] = inner.y;
    occaKernelArgs[5] = inner.x;

    int occaInnerId0 = 0, occaInnerId1 = 0, occaInnerId2 = 0;

    tmpKernel(occaKernelArgs,
              occaInnerId0, occaInnerId1, occaInnerId2,
              arg0.data(),
              arg1.data(),
              arg2.data(),
              arg3.data(),
              arg4.data(),
              arg5.data(),
              arg6.data(),
              arg7.data(),
              arg8.data(),
              arg9.data());
  }

  template <>
  void kernel_t<Serial>::operator () (const kernelArg &arg0,  const kernelArg &arg1,  const kernelArg &arg2, 
                      const kernelArg &arg3,  const kernelArg &arg4,  const kernelArg &arg5, 
                      const kernelArg &arg6,  const kernelArg &arg7,  const kernelArg &arg8, 
                      const kernelArg &arg9,  const kernelArg &arg10){
    SerialKernelData_t &data_ = *((SerialKernelData_t*) data);
    handleFunction_t tmpKernel = (handleFunction_t) data_.handle;
    int occaKernelArgs[6];

    occaKernelArgs[0] = outer.z;
    occaKernelArgs[1] = outer.y;
    occaKernelArgs[2] = outer.x;
    occaKernelArgs[3] = inner.z;
    occaKernelArgs[4] = inner.y;
    occaKernelArgs[5] = inner.x;

    int occaInnerId0 = 0, occaInnerId1 = 0, occaInnerId2 = 0;

    tmpKernel(occaKernelArgs,
              occaInnerId0, occaInnerId1, occaInnerId2,
              arg0.data(),
              arg1.data(),
              arg2.data(),
              arg3.data(),
              arg4.data(),
              arg5.data(),
              arg6.data(),
              arg7.data(),
              arg8.data(),
              arg9.data(),
              arg10.data());
  }

  template <>
  void kernel_t<Serial>::operator () (const kernelArg &arg0,  const kernelArg &arg1,  const kernelArg &arg2, 
                      const kernelArg &arg3,  const kernelArg &arg4,  const kernelArg &arg5, 
                      const kernelArg &arg6,  const kernelArg &arg7,  const kernelArg &arg8, 
                      const kernelArg &arg9,  const kernelArg &arg10,  const kernelArg &arg11){
    SerialKernelData_t &data_ = *((SerialKernelData_t*) data);
    handleFunction_t tmpKernel = (handleFunction_t) data_.handle;
    int occaKernelArgs[6];

    occaKernelArgs[0] = outer.z;
    occaKernelArgs[1] = outer.y;
    occaKernelArgs[2] = outer.x;
    occaKernelArgs[3] = inner.z;
    occaKernelArgs[4] = inner.y;
    occaKernelArgs[5] = inner.x;

    int occaInnerId0 = 0, occaInnerId1 = 0, occaInnerId2 = 0;

    tmpKernel(occaKernelArgs,
              occaInnerId0, occaInnerId1, occaInnerId2,
              arg0.data(),
              arg1.data(),
              arg2.data(),
              arg3.data(),
              arg4.data(),
              arg5.data(),
              arg6.data(),
              arg7.data(),
              arg8.data(),
              arg9.data(),
              arg10.data(),
              arg11.data());
  }

  template <>
  void kernel_t<Serial>::operator () (const kernelArg &arg0,  const kernelArg &arg1,  const kernelArg &arg2, 
                      const kernelArg &arg3,  const kernelArg &arg4,  const kernelArg &arg5, 
                      const kernelArg &arg6,  const kernelArg &arg7,  const kernelArg &arg8, 
                      const kernelArg &arg9,  const kernelArg &arg10,  const kernelArg &arg11, 
                      const kernelArg &arg12){
    SerialKernelData_t &data_ = *((SerialKernelData_t*) data);
    handleFunction_t tmpKernel = (handleFunction_t) data_.handle;
    int occaKernelArgs[6];

    occaKernelArgs[0] = outer.z;
    occaKernelArgs[1] = outer.y;
    occaKernelArgs[2] = outer.x;
    occaKernelArgs[3] = inner.z;
    occaKernelArgs[4] = inner.y;
    occaKernelArgs[5] = inner.x;

    int occaInnerId0 = 0, occaInnerId1 = 0, occaInnerId2 = 0;

    tmpKernel(occaKernelArgs,
              occaInnerId0, occaInnerId1, occaInnerId2,
              arg0.data(),
              arg1.data(),
              arg2.data(),
              arg3.data(),
              arg4.data(),
              arg5.data(),
              arg6.data(),
              arg7.data(),
              arg8.data(),
              arg9.data(),
              arg10.data(),
              arg11.data(),
              arg12.data());
  }

  template <>
  void kernel_t<Serial>::operator () (const kernelArg &arg0,  const kernelArg &arg1,  const kernelArg &arg2, 
                      const kernelArg &arg3,  const kernelArg &arg4,  const kernelArg &arg5, 
                      const kernelArg &arg6,  const kernelArg &arg7,  const kernelArg &arg8, 
                      const kernelArg &arg9,  const kernelArg &arg10,  const kernelArg &arg11, 
                      const kernelArg &arg12,  const kernelArg &arg13){
    SerialKernelData_t &data_ = *((SerialKernelData_t*) data);
    handleFunction_t tmpKernel = (handleFunction_t) data_.handle;
    int occaKernelArgs[6];

    occaKernelArgs[0] = outer.z;
    occaKernelArgs[1] = outer.y;
    occaKernelArgs[2] = outer.x;
    occaKernelArgs[3] = inner.z;
    occaKernelArgs[4] = inner.y;
    occaKernelArgs[5] = inner.x;

    int occaInnerId0 = 0, occaInnerId1 = 0, occaInnerId2 = 0;

    tmpKernel(occaKernelArgs,
              occaInnerId0, occaInnerId1, occaInnerId2,
              arg0.data(),
              arg1.data(),
              arg2.data(),
              arg3.data(),
              arg4.data(),
              arg5.data(),
              arg6.data(),
              arg7.data(),
              arg8.data(),
              arg9.data(),
              arg10.data(),
              arg11.data(),
              arg12.data(),
              arg13.data());
  }

  template <>
  void kernel_t<Serial>::operator () (const kernelArg &arg0,  const kernelArg &arg1,  const kernelArg &arg2, 
                      const kernelArg &arg3,  const kernelArg &arg4,  const kernelArg &arg5, 
                      const kernelArg &arg6,  const kernelArg &arg7,  const kernelArg &arg8, 
                      const kernelArg &arg9,  const kernelArg &arg10,  const kernelArg &arg11, 
                      const kernelArg &arg12,  const kernelArg &arg13,  const kernelArg &arg14){
    SerialKernelData_t &data_ = *((SerialKernelData_t*) data);
    handleFunction_t tmpKernel = (handleFunction_t) data_.handle;
    int occaKernelArgs[6];

    occaKernelArgs[0] = outer.z;
    occaKernelArgs[1] = outer.y;
    occaKernelArgs[2] = outer.x;
    occaKernelArgs[3] = inner.z;
    occaKernelArgs[4] = inner.y;
    occaKernelArgs[5] = inner.x;

    int occaInnerId0 = 0, occaInnerId1 = 0, occaInnerId2 = 0;

    tmpKernel(occaKernelArgs,
              occaInnerId0, occaInnerId1, occaInnerId2,
              arg0.data(),
              arg1.data(),
              arg2.data(),
              arg3.data(),
              arg4.data(),
              arg5.data(),
              arg6.data(),
              arg7.data(),
              arg8.data(),
              arg9.data(),
              arg10.data(),
              arg11.data(),
              arg12.data(),
              arg13.data(),
              arg14.data());
  }

  template <>
  void kernel_t<Serial>::operator () (const kernelArg &arg0,  const kernelArg &arg1,  const kernelArg &arg2, 
                      const kernelArg &arg3,  const kernelArg &arg4,  const kernelArg &arg5, 
                      const kernelArg &arg6,  const kernelArg &arg7,  const kernelArg &arg8, 
                      const kernelArg &arg9,  const kernelArg &arg10,  const kernelArg &arg11, 
                      const kernelArg &arg12,  const kernelArg &arg13,  const kernelArg &arg14, 
                      const kernelArg &arg15){
    SerialKernelData_t &data_ = *((SerialKernelData_t*) data);
    handleFunction_t tmpKernel = (handleFunction_t) data_.handle;
    int occaKernelArgs[6];

    occaKernelArgs[0] = outer.z;
    occaKernelArgs[1] = outer.y;
    occaKernelArgs[2] = outer.x;
    occaKernelArgs[3] = inner.z;
    occaKernelArgs[4] = inner.y;
    occaKernelArgs[5] = inner.x;

    int occaInnerId0 = 0, occaInnerId1 = 0, occaInnerId2 = 0;

    tmpKernel(occaKernelArgs,
              occaInnerId0, occaInnerId1, occaInnerId2,
              arg0.data(),
              arg1.data(),
              arg2.data(),
              arg3.data(),
              arg4.data(),
              arg5.data(),
              arg6.data(),
              arg7.data(),
              arg8.data(),
              arg9.data(),
              arg10.data(),
              arg11.data(),
              arg12.data(),
              arg13.data(),
              arg14.data(),
              arg15.data());
  }

  template <>
  void kernel_t<Serial>::operator () (const kernelArg &arg0,  const kernelArg &arg1,  const kernelArg &arg2, 
                      const kernelArg &arg3,  const kernelArg &arg4,  const kernelArg &arg5, 
                      const kernelArg &arg6,  const kernelArg &arg7,  const kernelArg &arg8, 
                      const kernelArg &arg9,  const kernelArg &arg10,  const kernelArg &arg11, 
                      const kernelArg &arg12,  const kernelArg &arg13,  const kernelArg &arg14, 
                      const kernelArg &arg15,  const kernelArg &arg16){
    SerialKernelData_t &data_ = *((SerialKernelData_t*) data);
    handleFunction_t tmpKernel = (handleFunction_t) data_.handle;
    int occaKernelArgs[6];

    occaKernelArgs[0] = outer.z;
    occaKernelArgs[1] = outer.y;
    occaKernelArgs[2] = outer.x;
    occaKernelArgs[3] = inner.z;
    occaKernelArgs[4] = inner.y;
    occaKernelArgs[5] = inner.x;

    int occaInnerId0 = 0, occaInnerId1 = 0, occaInnerId2 = 0;

    tmpKernel(occaKernelArgs,
              occaInnerId0, occaInnerId1, occaInnerId2,
              arg0.data(),
              arg1.data(),
              arg2.data(),
              arg3.data(),
              arg4.data(),
              arg5.data(),
              arg6.data(),
              arg7.data(),
              arg8.data(),
              arg9.data(),
              arg10.data(),
              arg11.data(),
              arg12.data(),
              arg13.data(),
              arg14.data(),
              arg15.data(),
              arg16.data());
  }

  template <>
  void kernel_t<Serial>::operator () (const kernelArg &arg0,  const kernelArg &arg1,  const kernelArg &arg2, 
                      const kernelArg &arg3,  const kernelArg &arg4,  const kernelArg &arg5, 
                      const kernelArg &arg6,  const kernelArg &arg7,  const kernelArg &arg8, 
                      const kernelArg &arg9,  const kernelArg &arg10,  const kernelArg &arg11, 
                      const kernelArg &arg12,  const kernelArg &arg13,  const kernelArg &arg14, 
                      const kernelArg &arg15,  const kernelArg &arg16,  const kernelArg &arg17){
    SerialKernelData_t &data_ = *((SerialKernelData_t*) data);
    handleFunction_t tmpKernel = (handleFunction_t) data_.handle;
    int occaKernelArgs[6];

    occaKernelArgs[0] = outer.z;
    occaKernelArgs[1] = outer.y;
    occaKernelArgs[2] = outer.x;
    occaKernelArgs[3] = inner.z;
    occaKernelArgs[4] = inner.y;
    occaKernelArgs[5] = inner.x;

    int occaInnerId0 = 0, occaInnerId1 = 0, occaInnerId2 = 0;

    tmpKernel(occaKernelArgs,
              occaInnerId0, occaInnerId1, occaInnerId2,
              arg0.data(),
              arg1.data(),
              arg2.data(),
              arg3.data(),
              arg4.data(),
              arg5.data(),
              arg6.data(),
              arg7.data(),
              arg8.data(),
              arg9.data(),
              arg10.data(),
              arg11.data(),
              arg12.data(),
              arg13.data(),
              arg14.data(),
              arg15.data(),
              arg16.data(),
              arg17.data());
  }

  template <>
  void kernel_t<Serial>::operator () (const kernelArg &arg0,  const kernelArg &arg1,  const kernelArg &arg2, 
                      const kernelArg &arg3,  const kernelArg &arg4,  const kernelArg &arg5, 
                      const kernelArg &arg6,  const kernelArg &arg7,  const kernelArg &arg8, 
                      const kernelArg &arg9,  const kernelArg &arg10,  const kernelArg &arg11, 
                      const kernelArg &arg12,  const kernelArg &arg13,  const kernelArg &arg14, 
                      const kernelArg &arg15,  const kernelArg &arg16,  const kernelArg &arg17, 
                      const kernelArg &arg18){
    SerialKernelData_t &data_ = *((SerialKernelData_t*) data);
    handleFunction_t tmpKernel = (handleFunction_t) data_.handle;
    int occaKernelArgs[6];

    occaKernelArgs[0] = outer.z;
    occaKernelArgs[1] = outer.y;
    occaKernelArgs[2] = outer.x;
    occaKernelArgs[3] = inner.z;
    occaKernelArgs[4] = inner.y;
    occaKernelArgs[5] = inner.x;

    int occaInnerId0 = 0, occaInnerId1 = 0, occaInnerId2 = 0;

    tmpKernel(occaKernelArgs,
              occaInnerId0, occaInnerId1, occaInnerId2,
              arg0.data(),
              arg1.data(),
              arg2.data(),
              arg3.data(),
              arg4.data(),
              arg5.data(),
              arg6.data(),
              arg7.data(),
              arg8.data(),
              arg9.data(),
              arg10.data(),
              arg11.data(),
              arg12.data(),
              arg13.data(),
              arg14.data(),
              arg15.data(),
              arg16.data(),
              arg17.data(),
              arg18.data());
  }

  template <>
  void kernel_t<Serial>::operator () (const kernelArg &arg0,  const kernelArg &arg1,  const kernelArg &arg2, 
                      const kernelArg &arg3,  const kernelArg &arg4,  const kernelArg &arg5, 
                      const kernelArg &arg6,  const kernelArg &arg7,  const kernelArg &arg8, 
                      const kernelArg &arg9,  const kernelArg &arg10,  const kernelArg &arg11, 
                      const kernelArg &arg12,  const kernelArg &arg13,  const kernelArg &arg14, 
                      const kernelArg &arg15,  const kernelArg &arg16,  const kernelArg &arg17, 
                      const kernelArg &arg18,  const kernelArg &arg19){
    SerialKernelData_t &data_ = *((SerialKernelData_t*) data);
    handleFunction_t tmpKernel = (handleFunction_t) data_.handle;
    int occaKernelArgs[6];

    occaKernelArgs[0] = outer.z;
    occaKernelArgs[1] = outer.y;
    occaKernelArgs[2] = outer.x;
    occaKernelArgs[3] = inner.z;
    occaKernelArgs[4] = inner.y;
    occaKernelArgs[5] = inner.x;

    int occaInnerId0 = 0, occaInnerId1 = 0, occaInnerId2 = 0;

    tmpKernel(occaKernelArgs,
              occaInnerId0, occaInnerId1, occaInnerId2,
              arg0.data(),
              arg1.data(),
              arg2.data(),
              arg3.data(),
              arg4.data(),
              arg5.data(),
              arg6.data(),
              arg7.data(),
              arg8.data(),
              arg9.data(),
              arg10.data(),
              arg11.data(),
              arg12.data(),
              arg13.data(),
              arg14.data(),
              arg15.data(),
              arg16.data(),
              arg17.data(),
              arg18.data(),
              arg19.data());
  }

  template <>
  void kernel_t<Serial>::operator () (const kernelArg &arg0,  const kernelArg &arg1,  const kernelArg &arg2, 
                      const kernelArg &arg3,  const kernelArg &arg4,  const kernelArg &arg5, 
                      const kernelArg &arg6,  const kernelArg &arg7,  const kernelArg &arg8, 
                      const kernelArg &arg9,  const kernelArg &arg10,  const kernelArg &arg11, 
                      const kernelArg &arg12,  const kernelArg &arg13,  const kernelArg &arg14, 
                      const kernelArg &arg15,  const kernelArg &arg16,  const kernelArg &arg17, 
                      const kernelArg &arg18,  const kernelArg &arg19,  const kernelArg &arg20){
    SerialKernelData_t &data_ = *((SerialKernelData_t*) data);
    handleFunction_t tmpKernel = (handleFunction_t) data_.handle;
    int occaKernelArgs[6];

    occaKernelArgs[0] = outer.z;
    occaKernelArgs[1] = outer.y;
    occaKernelArgs[2] = outer.x;
    occaKernelArgs[3] = inner.z;
    occaKernelArgs[4] = inner.y;
    occaKernelArgs[5] = inner.x;

    int occaInnerId0 = 0, occaInnerId1 = 0, occaInnerId2 = 0;

    tmpKernel(occaKernelArgs,
              occaInnerId0, occaInnerId1, occaInnerId2,
              arg0.data(),
              arg1.data(),
              arg2.data(),
              arg3.data(),
              arg4.data(),
              arg5.data(),
              arg6.data(),
              arg7.data(),
              arg8.data(),
              arg9.data(),
              arg10.data(),
              arg11.data(),
              arg12.data(),
              arg13.data(),
              arg14.data(),
              arg15.data(),
              arg16.data(),
              arg17.data(),
              arg18.data(),
              arg19.data(),
              arg20.data());
  }

  template <>
  void kernel_t<Serial>::operator () (const kernelArg &arg0,  const kernelArg &arg1,  const kernelArg &arg2, 
                      const kernelArg &arg3,  const kernelArg &arg4,  const kernelArg &arg5, 
                      const kernelArg &arg6,  const kernelArg &arg7,  const kernelArg &arg8, 
                      const kernelArg &arg9,  const kernelArg &arg10,  const kernelArg &arg11, 
                      const kernelArg &arg12,  const kernelArg &arg13,  const kernelArg &arg14, 
                      const kernelArg &arg15,  const kernelArg &arg16,  const kernelArg &arg17, 
                      const kernelArg &arg18,  const kernelArg &arg19,  const kernelArg &arg20, 
                      const kernelArg &arg21){
    SerialKernelData_t &data_ = *((SerialKernelData_t*) data);
    handleFunction_t tmpKernel = (handleFunction_t) data_.handle;
    int occaKernelArgs[6];

    occaKernelArgs[0] = outer.z;
    occaKernelArgs[1] = outer.y;
    occaKernelArgs[2] = outer.x;
    occaKernelArgs[3] = inner.z;
    occaKernelArgs[4] = inner.y;
    occaKernelArgs[5] = inner.x;

    int occaInnerId0 = 0, occaInnerId1 = 0, occaInnerId2 = 0;

    tmpKernel(occaKernelArgs,
              occaInnerId0, occaInnerId1, occaInnerId2,
              arg0.data(),
              arg1.data(),
              arg2.data(),
              arg3.data(),
              arg4.data(),
              arg5.data(),
              arg6.data(),
              arg7.data(),
              arg8.data(),
              arg9.data(),
              arg10.data(),
              arg11.data(),
              arg12.data(),
              arg13.data(),
              arg14.data(),
              arg15.data(),
              arg16.data(),
              arg17.data(),
              arg18.data(),
              arg19.data(),
              arg20.data(),
              arg21.data());
  }

  template <>
  void kernel_t<Serial>::operator () (const kernelArg &arg0,  const kernelArg &arg1,  const kernelArg &arg2, 
                      const kernelArg &arg3,  const kernelArg &arg4,  const kernelArg &arg5, 
                      const kernelArg &arg6,  const kernelArg &arg7,  const kernelArg &arg8, 
                      const kernelArg &arg9,  const kernelArg &arg10,  const kernelArg &arg11, 
                      const kernelArg &arg12,  const kernelArg &arg13,  const kernelArg &arg14, 
                      const kernelArg &arg15,  const kernelArg &arg16,  const kernelArg &arg17, 
                      const kernelArg &arg18,  const kernelArg &arg19,  const kernelArg &arg20, 
                      const kernelArg &arg21,  const kernelArg &arg22){
    SerialKernelData_t &data_ = *((SerialKernelData_t*) data);
    handleFunction_t tmpKernel = (handleFunction_t) data_.handle;
    int occaKernelArgs[6];

    occaKernelArgs[0] = outer.z;
    occaKernelArgs[1] = outer.y;
    occaKernelArgs[2] = outer.x;
    occaKernelArgs[3] = inner.z;
    occaKernelArgs[4] = inner.y;
    occaKernelArgs[5] = inner.x;

    int occaInnerId0 = 0, occaInnerId1 = 0, occaInnerId2 = 0;

    tmpKernel(occaKernelArgs,
              occaInnerId0, occaInnerId1, occaInnerId2,
              arg0.data(),
              arg1.data(),
              arg2.data(),
              arg3.data(),
              arg4.data(),
              arg5.data(),
              arg6.data(),
              arg7.data(),
              arg8.data(),
              arg9.data(),
              arg10.data(),
              arg11.data(),
              arg12.data(),
              arg13.data(),
              arg14.data(),
              arg15.data(),
              arg16.data(),
              arg17.data(),
              arg18.data(),
              arg19.data(),
              arg20.data(),
              arg21.data(),
              arg22.data());
  }

  template <>
  void kernel_t<Serial>::operator () (const kernelArg &arg0,  const kernelArg &arg1,  const kernelArg &arg2, 
                      const kernelArg &arg3,  const kernelArg &arg4,  const kernelArg &arg5, 
                      const kernelArg &arg6,  const kernelArg &arg7,  const kernelArg &arg8, 
                      const kernelArg &arg9,  const kernelArg &arg10,  const kernelArg &arg11, 
                      const kernelArg &arg12,  const kernelArg &arg13,  const kernelArg &arg14, 
                      const kernelArg &arg15,  const kernelArg &arg16,  const kernelArg &arg17, 
                      const kernelArg &arg18,  const kernelArg &arg19,  const kernelArg &arg20, 
                      const kernelArg &arg21,  const kernelArg &arg22,  const kernelArg &arg23){
    SerialKernelData_t &data_ = *((SerialKernelData_t*) data);
    handleFunction_t tmpKernel = (handleFunction_t) data_.handle;
    int occaKernelArgs[6];

    occaKernelArgs[0] = outer.z;
    occaKernelArgs[1] = outer.y;
    occaKernelArgs[2] = outer.x;
    occaKernelArgs[3] = inner.z;
    occaKernelArgs[4] = inner.y;
    occaKernelArgs[5] = inner.x;

    int occaInnerId0 = 0, occaInnerId1 = 0, occaInnerId2 = 0;

    tmpKernel(occaKernelArgs,
              occaInnerId0, occaInnerId1, occaInnerId2,
              arg0.data(),
              arg1.data(),
              arg2.data(),
              arg3.data(),
              arg4.data(),
              arg5.data(),
              arg6.data(),
              arg7.data(),
              arg8.data(),
              arg9.data(),
              arg10.data(),
              arg11.data(),
              arg12.data(),
              arg13.data(),
              arg14.data(),
              arg15.data(),
              arg16.data(),
              arg17.data(),
              arg18.data(),
              arg19.data(),
              arg20.data(),
              arg21.data(),
              arg22.data(),
              arg23.data());
  }

  template <>
  void kernel_t<Serial>::operator () (const kernelArg &arg0,  const kernelArg &arg1,  const kernelArg &arg2, 
                      const kernelArg &arg3,  const kernelArg &arg4,  const kernelArg &arg5, 
                      const kernelArg &arg6,  const kernelArg &arg7,  const kernelArg &arg8, 
                      const kernelArg &arg9,  const kernelArg &arg10,  const kernelArg &arg11, 
                      const kernelArg &arg12,  const kernelArg &arg13,  const kernelArg &arg14, 
                      const kernelArg &arg15,  const kernelArg &arg16,  const kernelArg &arg17, 
                      const kernelArg &arg18,  const kernelArg &arg19,  const kernelArg &arg20, 
                      const kernelArg &arg21,  const kernelArg &arg22,  const kernelArg &arg23, 
                      const kernelArg &arg24){
    SerialKernelData_t &data_ = *((SerialKernelData_t*) data);
    handleFunction_t tmpKernel = (handleFunction_t) data_.handle;
    int occaKernelArgs[6];

    occaKernelArgs[0] = outer.z;
    occaKernelArgs[1] = outer.y;
    occaKernelArgs[2] = outer.x;
    occaKernelArgs[3] = inner.z;
    occaKernelArgs[4] = inner.y;
    occaKernelArgs[5] = inner.x;

    int occaInnerId0 = 0, occaInnerId1 = 0, occaInnerId2 = 0;

    tmpKernel(occaKernelArgs,
              occaInnerId0, occaInnerId1, occaInnerId2,
              arg0.data(),
              arg1.data(),
              arg2.data(),
              arg3.data(),
              arg4.data(),
              arg5.data(),
              arg6.data(),
              arg7.data(),
              arg8.data(),
              arg9.data(),
              arg10.data(),
              arg11.data(),
              arg12.data(),
              arg13.data(),
              arg14.data(),
              arg15.data(),
              arg16.data(),
              arg17.data(),
              arg18.data(),
              arg19.data(),
              arg20.data(),
              arg21.data(),
              arg22.data(),
              arg23.data(),
              arg24.data());
  }

  template <>
  void kernel_t<Serial>::operator () (const kernelArg &arg0,  const kernelArg &arg1,  const kernelArg &arg2, 
                      const kernelArg &arg3,  const kernelArg &arg4,  const kernelArg &arg5, 
                      const kernelArg &arg6,  const kernelArg &arg7,  const kernelArg &arg8, 
                      const kernelArg &arg9,  const kernelArg &arg10,  const kernelArg &arg11, 
                      const kernelArg &arg12,  const kernelArg &arg13,  const kernelArg &arg14, 
                      const kernelArg &arg15,  const kernelArg &arg16,  const kernelArg &arg17, 
                      const kernelArg &arg18,  const kernelArg &arg19,  const kernelArg &arg20, 
                      const kernelArg &arg21,  const kernelArg &arg22,  const kernelArg &arg23, 
                      const kernelArg &arg24,  const kernelArg &arg25){
    SerialKernelData_t &data_ = *((SerialKernelData_t*) data);
    handleFunction_t tmpKernel = (handleFunction_t) data_.handle;
    int occaKernelArgs[6];

    occaKernelArgs[0] = outer.z;
    occaKernelArgs[1] = outer.y;
    occaKernelArgs[2] = outer.x;
    occaKernelArgs[3] = inner.z;
    occaKernelArgs[4] = inner.y;
    occaKernelArgs[5] = inner.x;

    int occaInnerId0 = 0, occaInnerId1 = 0, occaInnerId2 = 0;

    tmpKernel(occaKernelArgs,
              occaInnerId0, occaInnerId1, occaInnerId2,
              arg0.data(),
              arg1.data(),
              arg2.data(),
              arg3.data(),
              arg4.data(),
              arg5.data(),
              arg6.data(),
              arg7.data(),
              arg8.data(),
              arg9.data(),
              arg10.data(),
              arg11.data(),
              arg12.data(),
              arg13.data(),
              arg14.data(),
              arg15.data(),
              arg16.data(),
              arg17.data(),
              arg18.data(),
              arg19.data(),
              arg20.data(),
              arg21.data(),
              arg22.data(),
              arg23.data(),
              arg24.data(),
              arg25.data());
  }

  template <>
  void kernel_t<Serial>::operator () (const kernelArg &arg0,  const kernelArg &arg1,  const kernelArg &arg2, 
                      const kernelArg &arg3,  const kernelArg &arg4,  const kernelArg &arg5, 
                      const kernelArg &arg6,  const kernelArg &arg7,  const kernelArg &arg8, 
                      const kernelArg &arg9,  const kernelArg &arg10,  const kernelArg &arg11, 
                      const kernelArg &arg12,  const kernelArg &arg13,  const kernelArg &arg14, 
                      const kernelArg &arg15,  const kernelArg &arg16,  const kernelArg &arg17, 
                      const kernelArg &arg18,  const kernelArg &arg19,  const kernelArg &arg20, 
                      const kernelArg &arg21,  const kernelArg &arg22,  const kernelArg &arg23, 
                      const kernelArg &arg24,  const kernelArg &arg25,  const kernelArg &arg26){
    SerialKernelData_t &data_ = *((SerialKernelData_t*) data);
    handleFunction_t tmpKernel = (handleFunction_t) data_.handle;
    int occaKernelArgs[6];

    occaKernelArgs[0] = outer.z;
    occaKernelArgs[1] = outer.y;
    occaKernelArgs[2] = outer.x;
    occaKernelArgs[3] = inner.z;
    occaKernelArgs[4] = inner.y;
    occaKernelArgs[5] = inner.x;

    int occaInnerId0 = 0, occaInnerId1 = 0, occaInnerId2 = 0;

    tmpKernel(occaKernelArgs,
              occaInnerId0, occaInnerId1, occaInnerId2,
              arg0.data(),
              arg1.data(),
              arg2.data(),
              arg3.data(),
              arg4.data(),
              arg5.data(),
              arg6.data(),
              arg7.data(),
              arg8.data(),
              arg9.data(),
              arg10.data(),
              arg11.data(),
              arg12.data(),
              arg13.data(),
              arg14.data(),
              arg15.data(),
              arg16.data(),
              arg17.data(),
              arg18.data(),
              arg19.data(),
              arg20.data(),
              arg21.data(),
              arg22.data(),
              arg23.data(),
              arg24.data(),
              arg25.data(),
              arg26.data());
  }

  template <>
  void kernel_t<Serial>::operator () (const kernelArg &arg0,  const kernelArg &arg1,  const kernelArg &arg2, 
                      const kernelArg &arg3,  const kernelArg &arg4,  const kernelArg &arg5, 
                      const kernelArg &arg6,  const kernelArg &arg7,  const kernelArg &arg8, 
                      const kernelArg &arg9,  const kernelArg &arg10,  const kernelArg &arg11, 
                      const kernelArg &arg12,  const kernelArg &arg13,  const kernelArg &arg14, 
                      const kernelArg &arg15,  const kernelArg &arg16,  const kernelArg &arg17, 
                      const kernelArg &arg18,  const kernelArg &arg19,  const kernelArg &arg20, 
                      const kernelArg &arg21,  const kernelArg &arg22,  const kernelArg &arg23, 
                      const kernelArg &arg24,  const kernelArg &arg25,  const kernelArg &arg26, 
                      const kernelArg &arg27){
    SerialKernelData_t &data_ = *((SerialKernelData_t*) data);
    handleFunction_t tmpKernel = (handleFunction_t) data_.handle;
    int occaKernelArgs[6];

    occaKernelArgs[0] = outer.z;
    occaKernelArgs[1] = outer.y;
    occaKernelArgs[2] = outer.x;
    occaKernelArgs[3] = inner.z;
    occaKernelArgs[4] = inner.y;
    occaKernelArgs[5] = inner.x;

    int occaInnerId0 = 0, occaInnerId1 = 0, occaInnerId2 = 0;

    tmpKernel(occaKernelArgs,
              occaInnerId0, occaInnerId1, occaInnerId2,
              arg0.data(),
              arg1.data(),
              arg2.data(),
              arg3.data(),
              arg4.data(),
              arg5.data(),
              arg6.data(),
              arg7.data(),
              arg8.data(),
              arg9.data(),
              arg10.data(),
              arg11.data(),
              arg12.data(),
              arg13.data(),
              arg14.data(),
              arg15.data(),
              arg16.data(),
              arg17.data(),
              arg18.data(),
              arg19.data(),
              arg20.data(),
              arg21.data(),
              arg22.data(),
              arg23.data(),
              arg24.data(),
              arg25.data(),
              arg26.data(),
              arg27.data());
  }

  template <>
  void kernel_t<Serial>::operator () (const kernelArg &arg0,  const kernelArg &arg1,  const kernelArg &arg2, 
                      const kernelArg &arg3,  const kernelArg &arg4,  const kernelArg &arg5, 
                      const kernelArg &arg6,  const kernelArg &arg7,  const kernelArg &arg8, 
                      const kernelArg &arg9,  const kernelArg &arg10,  const kernelArg &arg11, 
                      const kernelArg &arg12,  const kernelArg &arg13,  const kernelArg &arg14, 
                      const kernelArg &arg15,  const kernelArg &arg16,  const kernelArg &arg17, 
                      const kernelArg &arg18,  const kernelArg &arg19,  const kernelArg &arg20, 
                      const kernelArg &arg21,  const kernelArg &arg22,  const kernelArg &arg23, 
                      const kernelArg &arg24,  const kernelArg &arg25,  const kernelArg &arg26, 
                      const kernelArg &arg27,  const kernelArg &arg28){
    SerialKernelData_t &data_ = *((SerialKernelData_t*) data);
    handleFunction_t tmpKernel = (handleFunction_t) data_.handle;
    int occaKernelArgs[6];

    occaKernelArgs[0] = outer.z;
    occaKernelArgs[1] = outer.y;
    occaKernelArgs[2] = outer.x;
    occaKernelArgs[3] = inner.z;
    occaKernelArgs[4] = inner.y;
    occaKernelArgs[5] = inner.x;

    int occaInnerId0 = 0, occaInnerId1 = 0, occaInnerId2 = 0;

    tmpKernel(occaKernelArgs,
              occaInnerId0, occaInnerId1, occaInnerId2,
              arg0.data(),
              arg1.data(),
              arg2.data(),
              arg3.data(),
              arg4.data(),
              arg5.data(),
              arg6.data(),
              arg7.data(),
              arg8.data(),
              arg9.data(),
              arg10.data(),
              arg11.data(),
              arg12.data(),
              arg13.data(),
              arg14.data(),
              arg15.data(),
              arg16.data(),
              arg17.data(),
              arg18.data(),
              arg19.data(),
              arg20.data(),
              arg21.data(),
              arg22.data(),
              arg23.data(),
              arg24.data(),
              arg25.data(),
              arg26.data(),
              arg27.data(),
              arg28.data());
  }

  template <>
  void kernel_t<Serial>::operator () (const kernelArg &arg0,  const kernelArg &arg1,  const kernelArg &arg2, 
                      const kernelArg &arg3,  const kernelArg &arg4,  const kernelArg &arg5, 
                      const kernelArg &arg6,  const kernelArg &arg7,  const kernelArg &arg8, 
                      const kernelArg &arg9,  const kernelArg &arg10,  const kernelArg &arg11, 
                      const kernelArg &arg12,  const kernelArg &arg13,  const kernelArg &arg14, 
                      const kernelArg &arg15,  const kernelArg &arg16,  const kernelArg &arg17, 
                      const kernelArg &arg18,  const kernelArg &arg19,  const kernelArg &arg20, 
                      const kernelArg &arg21,  const kernelArg &arg22,  const kernelArg &arg23, 
                      const kernelArg &arg24,  const kernelArg &arg25,  const kernelArg &arg26, 
                      const kernelArg &arg27,  const kernelArg &arg28,  const kernelArg &arg29){
    SerialKernelData_t &data_ = *((SerialKernelData_t*) data);
    handleFunction_t tmpKernel = (handleFunction_t) data_.handle;
    int occaKernelArgs[6];

    occaKernelArgs[0] = outer.z;
    occaKernelArgs[1] = outer.y;
    occaKernelArgs[2] = outer.x;
    occaKernelArgs[3] = inner.z;
    occaKernelArgs[4] = inner.y;
    occaKernelArgs[5] = inner.x;

    int occaInnerId0 = 0, occaInnerId1 = 0, occaInnerId2 = 0;

    tmpKernel(occaKernelArgs,
              occaInnerId0, occaInnerId1, occaInnerId2,
              arg0.data(),
              arg1.data(),
              arg2.data(),
              arg3.data(),
              arg4.data(),
              arg5.data(),
              arg6.data(),
              arg7.data(),
              arg8.data(),
              arg9.data(),
              arg10.data(),
              arg11.data(),
              arg12.data(),
              arg13.data(),
              arg14.data(),
              arg15.data(),
              arg16.data(),
              arg17.data(),
              arg18.data(),
              arg19.data(),
              arg20.data(),
              arg21.data(),
              arg22.data(),
              arg23.data(),
              arg24.data(),
              arg25.data(),
              arg26.data(),
              arg27.data(),
              arg28.data(),
              arg29.data());
  }

  template <>
  void kernel_t<Serial>::operator () (const kernelArg &arg0,  const kernelArg &arg1,  const kernelArg &arg2, 
                      const kernelArg &arg3,  const kernelArg &arg4,  const kernelArg &arg5, 
                      const kernelArg &arg6,  const kernelArg &arg7,  const kernelArg &arg8, 
                      const kernelArg &arg9,  const kernelArg &arg10,  const kernelArg &arg11, 
                      const kernelArg &arg12,  const kernelArg &arg13,  const kernelArg &arg14, 
                      const kernelArg &arg15,  const kernelArg &arg16,  const kernelArg &arg17, 
                      const kernelArg &arg18,  const kernelArg &arg19,  const kernelArg &arg20, 
                      const kernelArg &arg21,  const kernelArg &arg22,  const kernelArg &arg23, 
                      const kernelArg &arg24,  const kernelArg &arg25,  const kernelArg &arg26, 
                      const kernelArg &arg27,  const kernelArg &arg28,  const kernelArg &arg29, 
                      const kernelArg &arg30){
    SerialKernelData_t &data_ = *((SerialKernelData_t*) data);
    handleFunction_t tmpKernel = (handleFunction_t) data_.handle;
    int occaKernelArgs[6];

    occaKernelArgs[0] = outer.z;
    occaKernelArgs[1] = outer.y;
    occaKernelArgs[2] = outer.x;
    occaKernelArgs[3] = inner.z;
    occaKernelArgs[4] = inner.y;
    occaKernelArgs[5] = inner.x;

    int occaInnerId0 = 0, occaInnerId1 = 0, occaInnerId2 = 0;

    tmpKernel(occaKernelArgs,
              occaInnerId0, occaInnerId1, occaInnerId2,
              arg0.data(),
              arg1.data(),
              arg2.data(),
              arg3.data(),
              arg4.data(),
              arg5.data(),
              arg6.data(),
              arg7.data(),
              arg8.data(),
              arg9.data(),
              arg10.data(),
              arg11.data(),
              arg12.data(),
              arg13.data(),
              arg14.data(),
              arg15.data(),
              arg16.data(),
              arg17.data(),
              arg18.data(),
              arg19.data(),
              arg20.data(),
              arg21.data(),
              arg22.data(),
              arg23.data(),
              arg24.data(),
              arg25.data(),
              arg26.data(),
              arg27.data(),
              arg28.data(),
              arg29.data(),
              arg30.data());
  }

  template <>
  void kernel_t<Serial>::operator () (const kernelArg &arg0,  const kernelArg &arg1,  const kernelArg &arg2, 
                      const kernelArg &arg3,  const kernelArg &arg4,  const kernelArg &arg5, 
                      const kernelArg &arg6,  const kernelArg &arg7,  const kernelArg &arg8, 
                      const kernelArg &arg9,  const kernelArg &arg10,  const kernelArg &arg11, 
                      const kernelArg &arg12,  const kernelArg &arg13,  const kernelArg &arg14, 
                      const kernelArg &arg15,  const kernelArg &arg16,  const kernelArg &arg17, 
                      const kernelArg &arg18,  const kernelArg &arg19,  const kernelArg &arg20, 
                      const kernelArg &arg21,  const kernelArg &arg22,  const kernelArg &arg23, 
                      const kernelArg &arg24,  const kernelArg &arg25,  const kernelArg &arg26, 
                      const kernelArg &arg27,  const kernelArg &arg28,  const kernelArg &arg29, 
                      const kernelArg &arg30,  const kernelArg &arg31){
    SerialKernelData_t &data_ = *((SerialKernelData_t*) data);
    handleFunction_t tmpKernel = (handleFunction_t) data_.handle;
    int occaKernelArgs[6];

    occaKernelArgs[0] = outer.z;
    occaKernelArgs[1] = outer.y;
    occaKernelArgs[2] = outer.x;
    occaKernelArgs[3] = inner.z;
    occaKernelArgs[4] = inner.y;
    occaKernelArgs[5] = inner.x;

    int occaInnerId0 = 0, occaInnerId1 = 0, occaInnerId2 = 0;

    tmpKernel(occaKernelArgs,
              occaInnerId0, occaInnerId1, occaInnerId2,
              arg0.data(),
              arg1.data(),
              arg2.data(),
              arg3.data(),
              arg4.data(),
              arg5.data(),
              arg6.data(),
              arg7.data(),
              arg8.data(),
              arg9.data(),
              arg10.data(),
              arg11.data(),
              arg12.data(),
              arg13.data(),
              arg14.data(),
              arg15.data(),
              arg16.data(),
              arg17.data(),
              arg18.data(),
              arg19.data(),
              arg20.data(),
              arg21.data(),
              arg22.data(),
              arg23.data(),
              arg24.data(),
              arg25.data(),
              arg26.data(),
              arg27.data(),
              arg28.data(),
              arg29.data(),
              arg30.data(),
              arg31.data());
  }

  template <>
  void kernel_t<Serial>::operator () (const kernelArg &arg0,  const kernelArg &arg1,  const kernelArg &arg2, 
                      const kernelArg &arg3,  const kernelArg &arg4,  const kernelArg &arg5, 
                      const kernelArg &arg6,  const kernelArg &arg7,  const kernelArg &arg8, 
                      const kernelArg &arg9,  const kernelArg &arg10,  const kernelArg &arg11, 
                      const kernelArg &arg12,  const kernelArg &arg13,  const kernelArg &arg14, 
                      const kernelArg &arg15,  const kernelArg &arg16,  const kernelArg &arg17, 
                      const kernelArg &arg18,  const kernelArg &arg19,  const kernelArg &arg20, 
                      const kernelArg &arg21,  const kernelArg &arg22,  const kernelArg &arg23, 
                      const kernelArg &arg24,  const kernelArg &arg25,  const kernelArg &arg26, 
                      const kernelArg &arg27,  const kernelArg &arg28,  const kernelArg &arg29, 
                      const kernelArg &arg30,  const kernelArg &arg31,  const kernelArg &arg32){
    SerialKernelData_t &data_ = *((SerialKernelData_t*) data);
    handleFunction_t tmpKernel = (handleFunction_t) data_.handle;
    int occaKernelArgs[6];

    occaKernelArgs[0] = outer.z;
    occaKernelArgs[1] = outer.y;
    occaKernelArgs[2] = outer.x;
    occaKernelArgs[3] = inner.z;
    occaKernelArgs[4] = inner.y;
    occaKernelArgs[5] = inner.x;

    int occaInnerId0 = 0, occaInnerId1 = 0, occaInnerId2 = 0;

    tmpKernel(occaKernelArgs,
              occaInnerId0, occaInnerId1, occaInnerId2,
              arg0.data(),
              arg1.data(),
              arg2.data(),
              arg3.data(),
              arg4.data(),
              arg5.data(),
              arg6.data(),
              arg7.data(),
              arg8.data(),
              arg9.data(),
              arg10.data(),
              arg11.data(),
              arg12.data(),
              arg13.data(),
              arg14.data(),
              arg15.data(),
              arg16.data(),
              arg17.data(),
              arg18.data(),
              arg19.data(),
              arg20.data(),
              arg21.data(),
              arg22.data(),
              arg23.data(),
              arg24.data(),
              arg25.data(),
              arg26.data(),
              arg27.data(),
              arg28.data(),
              arg29.data(),
              arg30.data(),
              arg31.data(),
              arg32.data());
  }

  template <>
  void kernel_t<Serial>::operator () (const kernelArg &arg0,  const kernelArg &arg1,  const kernelArg &arg2, 
                      const kernelArg &arg3,  const kernelArg &arg4,  const kernelArg &arg5, 
                      const kernelArg &arg6,  const kernelArg &arg7,  const kernelArg &arg8, 
                      const kernelArg &arg9,  const kernelArg &arg10,  const kernelArg &arg11, 
                      const kernelArg &arg12,  const kernelArg &arg13,  const kernelArg &arg14, 
                      const kernelArg &arg15,  const kernelArg &arg16,  const kernelArg &arg17, 
                      const kernelArg &arg18,  const kernelArg &arg19,  const kernelArg &arg20, 
                      const kernelArg &arg21,  const kernelArg &arg22,  const kernelArg &arg23, 
                      const kernelArg &arg24,  const kernelArg &arg25,  const kernelArg &arg26, 
                      const kernelArg &arg27,  const kernelArg &arg28,  const kernelArg &arg29, 
                      const kernelArg &arg30,  const kernelArg &arg31,  const kernelArg &arg32, 
                      const kernelArg &arg33){
    SerialKernelData_t &data_ = *((SerialKernelData_t*) data);
    handleFunction_t tmpKernel = (handleFunction_t) data_.handle;
    int occaKernelArgs[6];

    occaKernelArgs[0] = outer.z;
    occaKernelArgs[1] = outer.y;
    occaKernelArgs[2] = outer.x;
    occaKernelArgs[3] = inner.z;
    occaKernelArgs[4] = inner.y;
    occaKernelArgs[5] = inner.x;

    int occaInnerId0 = 0, occaInnerId1 = 0, occaInnerId2 = 0;

    tmpKernel(occaKernelArgs,
              occaInnerId0, occaInnerId1, occaInnerId2,
              arg0.data(),
              arg1.data(),
              arg2.data(),
              arg3.data(),
              arg4.data(),
              arg5.data(),
              arg6.data(),
              arg7.data(),
              arg8.data(),
              arg9.data(),
              arg10.data(),
              arg11.data(),
              arg12.data(),
              arg13.data(),
              arg14.data(),
              arg15.data(),
              arg16.data(),
              arg17.data(),
              arg18.data(),
              arg19.data(),
              arg20.data(),
              arg21.data(),
              arg22.data(),
              arg23.data(),
              arg24.data(),
              arg25.data(),
              arg26.data(),
              arg27.data(),
              arg28.data(),
              arg29.data(),
              arg30.data(),
              arg31.data(),
              arg32.data(),
              arg33.data());
  }

  template <>
  void kernel_t<Serial>::operator () (const kernelArg &arg0,  const kernelArg &arg1,  const kernelArg &arg2, 
                      const kernelArg &arg3,  const kernelArg &arg4,  const kernelArg &arg5, 
                      const kernelArg &arg6,  const kernelArg &arg7,  const kernelArg &arg8, 
                      const kernelArg &arg9,  const kernelArg &arg10,  const kernelArg &arg11, 
                      const kernelArg &arg12,  const kernelArg &arg13,  const kernelArg &arg14, 
                      const kernelArg &arg15,  const kernelArg &arg16,  const kernelArg &arg17, 
                      const kernelArg &arg18,  const kernelArg &arg19,  const kernelArg &arg20, 
                      const kernelArg &arg21,  const kernelArg &arg22,  const kernelArg &arg23, 
                      const kernelArg &arg24,  const kernelArg &arg25,  const kernelArg &arg26, 
                      const kernelArg &arg27,  const kernelArg &arg28,  const kernelArg &arg29, 
                      const kernelArg &arg30,  const kernelArg &arg31,  const kernelArg &arg32, 
                      const kernelArg &arg33,  const kernelArg &arg34){
    SerialKernelData_t &data_ = *((SerialKernelData_t*) data);
    handleFunction_t tmpKernel = (handleFunction_t) data_.handle;
    int occaKernelArgs[6];

    occaKernelArgs[0] = outer.z;
    occaKernelArgs[1] = outer.y;
    occaKernelArgs[2] = outer.x;
    occaKernelArgs[3] = inner.z;
    occaKernelArgs[4] = inner.y;
    occaKernelArgs[5] = inner.x;

    int occaInnerId0 = 0, occaInnerId1 = 0, occaInnerId2 = 0;

    tmpKernel(occaKernelArgs,
              occaInnerId0, occaInnerId1, occaInnerId2,
              arg0.data(),
              arg1.data(),
              arg2.data(),
              arg3.data(),
              arg4.data(),
              arg5.data(),
              arg6.data(),
              arg7.data(),
              arg8.data(),
              arg9.data(),
              arg10.data(),
              arg11.data(),
              arg12.data(),
              arg13.data(),
              arg14.data(),
              arg15.data(),
              arg16.data(),
              arg17.data(),
              arg18.data(),
              arg19.data(),
              arg20.data(),
              arg21.data(),
              arg22.data(),
              arg23.data(),
              arg24.data(),
              arg25.data(),
              arg26.data(),
              arg27.data(),
              arg28.data(),
              arg29.data(),
              arg30.data(),
              arg31.data(),
              arg32.data(),
              arg33.data(),
              arg34.data());
  }

  template <>
  void kernel_t<Serial>::operator () (const kernelArg &arg0,  const kernelArg &arg1,  const kernelArg &arg2, 
                      const kernelArg &arg3,  const kernelArg &arg4,  const kernelArg &arg5, 
                      const kernelArg &arg6,  const kernelArg &arg7,  const kernelArg &arg8, 
                      const kernelArg &arg9,  const kernelArg &arg10,  const kernelArg &arg11, 
                      const kernelArg &arg12,  const kernelArg &arg13,  const kernelArg &arg14, 
                      const kernelArg &arg15,  const kernelArg &arg16,  const kernelArg &arg17, 
                      const kernelArg &arg18,  const kernelArg &arg19,  const kernelArg &arg20, 
                      const kernelArg &arg21,  const kernelArg &arg22,  const kernelArg &arg23, 
                      const kernelArg &arg24,  const kernelArg &arg25,  const kernelArg &arg26, 
                      const kernelArg &arg27,  const kernelArg &arg28,  const kernelArg &arg29, 
                      const kernelArg &arg30,  const kernelArg &arg31,  const kernelArg &arg32, 
                      const kernelArg &arg33,  const kernelArg &arg34,  const kernelArg &arg35){
    SerialKernelData_t &data_ = *((SerialKernelData_t*) data);
    handleFunction_t tmpKernel = (handleFunction_t) data_.handle;
    int occaKernelArgs[6];

    occaKernelArgs[0] = outer.z;
    occaKernelArgs[1] = outer.y;
    occaKernelArgs[2] = outer.x;
    occaKernelArgs[3] = inner.z;
    occaKernelArgs[4] = inner.y;
    occaKernelArgs[5] = inner.x;

    int occaInnerId0 = 0, occaInnerId1 = 0, occaInnerId2 = 0;

    tmpKernel(occaKernelArgs,
              occaInnerId0, occaInnerId1, occaInnerId2,
              arg0.data(),
              arg1.data(),
              arg2.data(),
              arg3.data(),
              arg4.data(),
              arg5.data(),
              arg6.data(),
              arg7.data(),
              arg8.data(),
              arg9.data(),
              arg10.data(),
              arg11.data(),
              arg12.data(),
              arg13.data(),
              arg14.data(),
              arg15.data(),
              arg16.data(),
              arg17.data(),
              arg18.data(),
              arg19.data(),
              arg20.data(),
              arg21.data(),
              arg22.data(),
              arg23.data(),
              arg24.data(),
              arg25.data(),
              arg26.data(),
              arg27.data(),
              arg28.data(),
              arg29.data(),
              arg30.data(),
              arg31.data(),
              arg32.data(),
              arg33.data(),
              arg34.data(),
              arg35.data());
  }

  template <>
  void kernel_t<Serial>::operator () (const kernelArg &arg0,  const kernelArg &arg1,  const kernelArg &arg2, 
                      const kernelArg &arg3,  const kernelArg &arg4,  const kernelArg &arg5, 
                      const kernelArg &arg6,  const kernelArg &arg7,  const kernelArg &arg8, 
                      const kernelArg &arg9,  const kernelArg &arg10,  const kernelArg &arg11, 
                      const kernelArg &arg12,  const kernelArg &arg13,  const kernelArg &arg14, 
                      const kernelArg &arg15,  const kernelArg &arg16,  const kernelArg &arg17, 
                      const kernelArg &arg18,  const kernelArg &arg19,  const kernelArg &arg20, 
                      const kernelArg &arg21,  const kernelArg &arg22,  const kernelArg &arg23, 
                      const kernelArg &arg24,  const kernelArg &arg25,  const kernelArg &arg26, 
                      const kernelArg &arg27,  const kernelArg &arg28,  const kernelArg &arg29, 
                      const kernelArg &arg30,  const kernelArg &arg31,  const kernelArg &arg32, 
                      const kernelArg &arg33,  const kernelArg &arg34,  const kernelArg &arg35, 
                      const kernelArg &arg36){
    SerialKernelData_t &data_ = *((SerialKernelData_t*) data);
    handleFunction_t tmpKernel = (handleFunction_t) data_.handle;
    int occaKernelArgs[6];

    occaKernelArgs[0] = outer.z;
    occaKernelArgs[1] = outer.y;
    occaKernelArgs[2] = outer.x;
    occaKernelArgs[3] = inner.z;
    occaKernelArgs[4] = inner.y;
    occaKernelArgs[5] = inner.x;

    int occaInnerId0 = 0, occaInnerId1 = 0, occaInnerId2 = 0;

    tmpKernel(occaKernelArgs,
              occaInnerId0, occaInnerId1, occaInnerId2,
              arg0.data(),
              arg1.data(),
              arg2.data(),
              arg3.data(),
              arg4.data(),
              arg5.data(),
              arg6.data(),
              arg7.data(),
              arg8.data(),
              arg9.data(),
              arg10.data(),
              arg11.data(),
              arg12.data(),
              arg13.data(),
              arg14.data(),
              arg15.data(),
              arg16.data(),
              arg17.data(),
              arg18.data(),
              arg19.data(),
              arg20.data(),
              arg21.data(),
              arg22.data(),
              arg23.data(),
              arg24.data(),
              arg25.data(),
              arg26.data(),
              arg27.data(),
              arg28.data(),
              arg29.data(),
              arg30.data(),
              arg31.data(),
              arg32.data(),
              arg33.data(),
              arg34.data(),
              arg35.data(),
              arg36.data());
  }

  template <>
  void kernel_t<Serial>::operator () (const kernelArg &arg0,  const kernelArg &arg1,  const kernelArg &arg2, 
                      const kernelArg &arg3,  const kernelArg &arg4,  const kernelArg &arg5, 
                      const kernelArg &arg6,  const kernelArg &arg7,  const kernelArg &arg8, 
                      const kernelArg &arg9,  const kernelArg &arg10,  const kernelArg &arg11, 
                      const kernelArg &arg12,  const kernelArg &arg13,  const kernelArg &arg14, 
                      const kernelArg &arg15,  const kernelArg &arg16,  const kernelArg &arg17, 
                      const kernelArg &arg18,  const kernelArg &arg19,  const kernelArg &arg20, 
                      const kernelArg &arg21,  const kernelArg &arg22,  const kernelArg &arg23, 
                      const kernelArg &arg24,  const kernelArg &arg25,  const kernelArg &arg26, 
                      const kernelArg &arg27,  const kernelArg &arg28,  const kernelArg &arg29, 
                      const kernelArg &arg30,  const kernelArg &arg31,  const kernelArg &arg32, 
                      const kernelArg &arg33,  const kernelArg &arg34,  const kernelArg &arg35, 
                      const kernelArg &arg36,  const kernelArg &arg37){
    SerialKernelData_t &data_ = *((SerialKernelData_t*) data);
    handleFunction_t tmpKernel = (handleFunction_t) data_.handle;
    int occaKernelArgs[6];

    occaKernelArgs[0] = outer.z;
    occaKernelArgs[1] = outer.y;
    occaKernelArgs[2] = outer.x;
    occaKernelArgs[3] = inner.z;
    occaKernelArgs[4] = inner.y;
    occaKernelArgs[5] = inner.x;

    int occaInnerId0 = 0, occaInnerId1 = 0, occaInnerId2 = 0;

    tmpKernel(occaKernelArgs,
              occaInnerId0, occaInnerId1, occaInnerId2,
              arg0.data(),
              arg1.data(),
              arg2.data(),
              arg3.data(),
              arg4.data(),
              arg5.data(),
              arg6.data(),
              arg7.data(),
              arg8.data(),
              arg9.data(),
              arg10.data(),
              arg11.data(),
              arg12.data(),
              arg13.data(),
              arg14.data(),
              arg15.data(),
              arg16.data(),
              arg17.data(),
              arg18.data(),
              arg19.data(),
              arg20.data(),
              arg21.data(),
              arg22.data(),
              arg23.data(),
              arg24.data(),
              arg25.data(),
              arg26.data(),
              arg27.data(),
              arg28.data(),
              arg29.data(),
              arg30.data(),
              arg31.data(),
              arg32.data(),
              arg33.data(),
              arg34.data(),
              arg35.data(),
              arg36.data(),
              arg37.data());
  }

  template <>
  void kernel_t<Serial>::operator () (const kernelArg &arg0,  const kernelArg &arg1,  const kernelArg &arg2, 
                      const kernelArg &arg3,  const kernelArg &arg4,  const kernelArg &arg5, 
                      const kernelArg &arg6,  const kernelArg &arg7,  const kernelArg &arg8, 
                      const kernelArg &arg9,  const kernelArg &arg10,  const kernelArg &arg11, 
                      const kernelArg &arg12,  const kernelArg &arg13,  const kernelArg &arg14, 
                      const kernelArg &arg15,  const kernelArg &arg16,  const kernelArg &arg17, 
                      const kernelArg &arg18,  const kernelArg &arg19,  const kernelArg &arg20, 
                      const kernelArg &arg21,  const kernelArg &arg22,  const kernelArg &arg23, 
                      const kernelArg &arg24,  const kernelArg &arg25,  const kernelArg &arg26, 
                      const kernelArg &arg27,  const kernelArg &arg28,  const kernelArg &arg29, 
                      const kernelArg &arg30,  const kernelArg &arg31,  const kernelArg &arg32, 
                      const kernelArg &arg33,  const kernelArg &arg34,  const kernelArg &arg35, 
                      const kernelArg &arg36,  const kernelArg &arg37,  const kernelArg &arg38){
    SerialKernelData_t &data_ = *((SerialKernelData_t*) data);
    handleFunction_t tmpKernel = (handleFunction_t) data_.handle;
    int occaKernelArgs[6];

    occaKernelArgs[0] = outer.z;
    occaKernelArgs[1] = outer.y;
    occaKernelArgs[2] = outer.x;
    occaKernelArgs[3] = inner.z;
    occaKernelArgs[4] = inner.y;
    occaKernelArgs[5] = inner.x;

    int occaInnerId0 = 0, occaInnerId1 = 0, occaInnerId2 = 0;

    tmpKernel(occaKernelArgs,
              occaInnerId0, occaInnerId1, occaInnerId2,
              arg0.data(),
              arg1.data(),
              arg2.data(),
              arg3.data(),
              arg4.data(),
              arg5.data(),
              arg6.data(),
              arg7.data(),
              arg8.data(),
              arg9.data(),
              arg10.data(),
              arg11.data(),
              arg12.data(),
              arg13.data(),
              arg14.data(),
              arg15.data(),
              arg16.data(),
              arg17.data(),
              arg18.data(),
              arg19.data(),
              arg20.data(),
              arg21.data(),
              arg22.data(),
              arg23.data(),
              arg24.data(),
              arg25.data(),
              arg26.data(),
              arg27.data(),
              arg28.data(),
              arg29.data(),
              arg30.data(),
              arg31.data(),
              arg32.data(),
              arg33.data(),
              arg34.data(),
              arg35.data(),
              arg36.data(),
              arg37.data(),
              arg38.data());
  }

  template <>
  void kernel_t<Serial>::operator () (const kernelArg &arg0,  const kernelArg &arg1,  const kernelArg &arg2, 
                      const kernelArg &arg3,  const kernelArg &arg4,  const kernelArg &arg5, 
                      const kernelArg &arg6,  const kernelArg &arg7,  const kernelArg &arg8, 
                      const kernelArg &arg9,  const kernelArg &arg10,  const kernelArg &arg11, 
                      const kernelArg &arg12,  const kernelArg &arg13,  const kernelArg &arg14, 
                      const kernelArg &arg15,  const kernelArg &arg16,  const kernelArg &arg17, 
                      const kernelArg &arg18,  const kernelArg &arg19,  const kernelArg &arg20, 
                      const kernelArg &arg21,  const kernelArg &arg22,  const kernelArg &arg23, 
                      const kernelArg &arg24,  const kernelArg &arg25,  const kernelArg &arg26, 
                      const kernelArg &arg27,  const kernelArg &arg28,  const kernelArg &arg29, 
                      const kernelArg &arg30,  const kernelArg &arg31,  const kernelArg &arg32, 
                      const kernelArg &arg33,  const kernelArg &arg34,  const kernelArg &arg35, 
                      const kernelArg &arg36,  const kernelArg &arg37,  const kernelArg &arg38, 
                      const kernelArg &arg39){
    SerialKernelData_t &data_ = *((SerialKernelData_t*) data);
    handleFunction_t tmpKernel = (handleFunction_t) data_.handle;
    int occaKernelArgs[6];

    occaKernelArgs[0] = outer.z;
    occaKernelArgs[1] = outer.y;
    occaKernelArgs[2] = outer.x;
    occaKernelArgs[3] = inner.z;
    occaKernelArgs[4] = inner.y;
    occaKernelArgs[5] = inner.x;

    int occaInnerId0 = 0, occaInnerId1 = 0, occaInnerId2 = 0;

    tmpKernel(occaKernelArgs,
              occaInnerId0, occaInnerId1, occaInnerId2,
              arg0.data(),
              arg1.data(),
              arg2.data(),
              arg3.data(),
              arg4.data(),
              arg5.data(),
              arg6.data(),
              arg7.data(),
              arg8.data(),
              arg9.data(),
              arg10.data(),
              arg11.data(),
              arg12.data(),
              arg13.data(),
              arg14.data(),
              arg15.data(),
              arg16.data(),
              arg17.data(),
              arg18.data(),
              arg19.data(),
              arg20.data(),
              arg21.data(),
              arg22.data(),
              arg23.data(),
              arg24.data(),
              arg25.data(),
              arg26.data(),
              arg27.data(),
              arg28.data(),
              arg29.data(),
              arg30.data(),
              arg31.data(),
              arg32.data(),
              arg33.data(),
              arg34.data(),
              arg35.data(),
              arg36.data(),
              arg37.data(),
              arg38.data(),
              arg39.data());
  }

  template <>
  void kernel_t<Serial>::operator () (const kernelArg &arg0,  const kernelArg &arg1,  const kernelArg &arg2, 
                      const kernelArg &arg3,  const kernelArg &arg4,  const kernelArg &arg5, 
                      const kernelArg &arg6,  const kernelArg &arg7,  const kernelArg &arg8, 
                      const kernelArg &arg9,  const kernelArg &arg10,  const kernelArg &arg11, 
                      const kernelArg &arg12,  const kernelArg &arg13,  const kernelArg &arg14, 
                      const kernelArg &arg15,  const kernelArg &arg16,  const kernelArg &arg17, 
                      const kernelArg &arg18,  const kernelArg &arg19,  const kernelArg &arg20, 
                      const kernelArg &arg21,  const kernelArg &arg22,  const kernelArg &arg23, 
                      const kernelArg &arg24,  const kernelArg &arg25,  const kernelArg &arg26, 
                      const kernelArg &arg27,  const kernelArg &arg28,  const kernelArg &arg29, 
                      const kernelArg &arg30,  const kernelArg &arg31,  const kernelArg &arg32, 
                      const kernelArg &arg33,  const kernelArg &arg34,  const kernelArg &arg35, 
                      const kernelArg &arg36,  const kernelArg &arg37,  const kernelArg &arg38, 
                      const kernelArg &arg39,  const kernelArg &arg40){
    SerialKernelData_t &data_ = *((SerialKernelData_t*) data);
    handleFunction_t tmpKernel = (handleFunction_t) data_.handle;
    int occaKernelArgs[6];

    occaKernelArgs[0] = outer.z;
    occaKernelArgs[1] = outer.y;
    occaKernelArgs[2] = outer.x;
    occaKernelArgs[3] = inner.z;
    occaKernelArgs[4] = inner.y;
    occaKernelArgs[5] = inner.x;

    int occaInnerId0 = 0, occaInnerId1 = 0, occaInnerId2 = 0;

    tmpKernel(occaKernelArgs,
              occaInnerId0, occaInnerId1, occaInnerId2,
              arg0.data(),
              arg1.data(),
              arg2.data(),
              arg3.data(),
              arg4.data(),
              arg5.data(),
              arg6.data(),
              arg7.data(),
              arg8.data(),
              arg9.data(),
              arg10.data(),
              arg11.data(),
              arg12.data(),
              arg13.data(),
              arg14.data(),
              arg15.data(),
              arg16.data(),
              arg17.data(),
              arg18.data(),
              arg19.data(),
              arg20.data(),
              arg21.data(),
              arg22.data(),
              arg23.data(),
              arg24.data(),
              arg25.data(),
              arg26.data(),
              arg27.data(),
              arg28.data(),
              arg29.data(),
              arg30.data(),
              arg31.data(),
              arg32.data(),
              arg33.data(),
              arg34.data(),
              arg35.data(),
              arg36.data(),
              arg37.data(),
              arg38.data(),
              arg39.data(),
              arg40.data());
  }

  template <>
  void kernel_t<Serial>::operator () (const kernelArg &arg0,  const kernelArg &arg1,  const kernelArg &arg2, 
                      const kernelArg &arg3,  const kernelArg &arg4,  const kernelArg &arg5, 
                      const kernelArg &arg6,  const kernelArg &arg7,  const kernelArg &arg8, 
                      const kernelArg &arg9,  const kernelArg &arg10,  const kernelArg &arg11, 
                      const kernelArg &arg12,  const kernelArg &arg13,  const kernelArg &arg14, 
                      const kernelArg &arg15,  const kernelArg &arg16,  const kernelArg &arg17, 
                      const kernelArg &arg18,  const kernelArg &arg19,  const kernelArg &arg20, 
                      const kernelArg &arg21,  const kernelArg &arg22,  const kernelArg &arg23, 
                      const kernelArg &arg24,  const kernelArg &arg25,  const kernelArg &arg26, 
                      const kernelArg &arg27,  const kernelArg &arg28,  const kernelArg &arg29, 
                      const kernelArg &arg30,  const kernelArg &arg31,  const kernelArg &arg32, 
                      const kernelArg &arg33,  const kernelArg &arg34,  const kernelArg &arg35, 
                      const kernelArg &arg36,  const kernelArg &arg37,  const kernelArg &arg38, 
                      const kernelArg &arg39,  const kernelArg &arg40,  const kernelArg &arg41){
    SerialKernelData_t &data_ = *((SerialKernelData_t*) data);
    handleFunction_t tmpKernel = (handleFunction_t) data_.handle;
    int occaKernelArgs[6];

    occaKernelArgs[0] = outer.z;
    occaKernelArgs[1] = outer.y;
    occaKernelArgs[2] = outer.x;
    occaKernelArgs[3] = inner.z;
    occaKernelArgs[4] = inner.y;
    occaKernelArgs[5] = inner.x;

    int occaInnerId0 = 0, occaInnerId1 = 0, occaInnerId2 = 0;

    tmpKernel(occaKernelArgs,
              occaInnerId0, occaInnerId1, occaInnerId2,
              arg0.data(),
              arg1.data(),
              arg2.data(),
              arg3.data(),
              arg4.data(),
              arg5.data(),
              arg6.data(),
              arg7.data(),
              arg8.data(),
              arg9.data(),
              arg10.data(),
              arg11.data(),
              arg12.data(),
              arg13.data(),
              arg14.data(),
              arg15.data(),
              arg16.data(),
              arg17.data(),
              arg18.data(),
              arg19.data(),
              arg20.data(),
              arg21.data(),
              arg22.data(),
              arg23.data(),
              arg24.data(),
              arg25.data(),
              arg26.data(),
              arg27.data(),
              arg28.data(),
              arg29.data(),
              arg30.data(),
              arg31.data(),
              arg32.data(),
              arg33.data(),
              arg34.data(),
              arg35.data(),
              arg36.data(),
              arg37.data(),
              arg38.data(),
              arg39.data(),
              arg40.data(),
              arg41.data());
  }

  template <>
  void kernel_t<Serial>::operator () (const kernelArg &arg0,  const kernelArg &arg1,  const kernelArg &arg2, 
                      const kernelArg &arg3,  const kernelArg &arg4,  const kernelArg &arg5, 
                      const kernelArg &arg6,  const kernelArg &arg7,  const kernelArg &arg8, 
                      const kernelArg &arg9,  const kernelArg &arg10,  const kernelArg &arg11, 
                      const kernelArg &arg12,  const kernelArg &arg13,  const kernelArg &arg14, 
                      const kernelArg &arg15,  const kernelArg &arg16,  const kernelArg &arg17, 
                      const kernelArg &arg18,  const kernelArg &arg19,  const kernelArg &arg20, 
                      const kernelArg &arg21,  const kernelArg &arg22,  const kernelArg &arg23, 
                      const kernelArg &arg24,  const kernelArg &arg25,  const kernelArg &arg26, 
                      const kernelArg &arg27,  const kernelArg &arg28,  const kernelArg &arg29, 
                      const kernelArg &arg30,  const kernelArg &arg31,  const kernelArg &arg32, 
                      const kernelArg &arg33,  const kernelArg &arg34,  const kernelArg &arg35, 
                      const kernelArg &arg36,  const kernelArg &arg37,  const kernelArg &arg38, 
                      const kernelArg &arg39,  const kernelArg &arg40,  const kernelArg &arg41, 
                      const kernelArg &arg42){
    SerialKernelData_t &data_ = *((SerialKernelData_t*) data);
    handleFunction_t tmpKernel = (handleFunction_t) data_.handle;
    int occaKernelArgs[6];

    occaKernelArgs[0] = outer.z;
    occaKernelArgs[1] = outer.y;
    occaKernelArgs[2] = outer.x;
    occaKernelArgs[3] = inner.z;
    occaKernelArgs[4] = inner.y;
    occaKernelArgs[5] = inner.x;

    int occaInnerId0 = 0, occaInnerId1 = 0, occaInnerId2 = 0;

    tmpKernel(occaKernelArgs,
              occaInnerId0, occaInnerId1, occaInnerId2,
              arg0.data(),
              arg1.data(),
              arg2.data(),
              arg3.data(),
              arg4.data(),
              arg5.data(),
              arg6.data(),
              arg7.data(),
              arg8.data(),
              arg9.data(),
              arg10.data(),
              arg11.data(),
              arg12.data(),
              arg13.data(),
              arg14.data(),
              arg15.data(),
              arg16.data(),
              arg17.data(),
              arg18.data(),
              arg19.data(),
              arg20.data(),
              arg21.data(),
              arg22.data(),
              arg23.data(),
              arg24.data(),
              arg25.data(),
              arg26.data(),
              arg27.data(),
              arg28.data(),
              arg29.data(),
              arg30.data(),
              arg31.data(),
              arg32.data(),
              arg33.data(),
              arg34.data(),
              arg35.data(),
              arg36.data(),
              arg37.data(),
              arg38.data(),
              arg39.data(),
              arg40.data(),
              arg41.data(),
              arg42.data());
  }

  template <>
  void kernel_t<Serial>::operator () (const kernelArg &arg0,  const kernelArg &arg1,  const kernelArg &arg2, 
                      const kernelArg &arg3,  const kernelArg &arg4,  const kernelArg &arg5, 
                      const kernelArg &arg6,  const kernelArg &arg7,  const kernelArg &arg8, 
                      const kernelArg &arg9,  const kernelArg &arg10,  const kernelArg &arg11, 
                      const kernelArg &arg12,  const kernelArg &arg13,  const kernelArg &arg14, 
                      const kernelArg &arg15,  const kernelArg &arg16,  const kernelArg &arg17, 
                      const kernelArg &arg18,  const kernelArg &arg19,  const kernelArg &arg20, 
                      const kernelArg &arg21,  const kernelArg &arg22,  const kernelArg &arg23, 
                      const kernelArg &arg24,  const kernelArg &arg25,  const kernelArg &arg26, 
                      const kernelArg &arg27,  const kernelArg &arg28,  const kernelArg &arg29, 
                      const kernelArg &arg30,  const kernelArg &arg31,  const kernelArg &arg32, 
                      const kernelArg &arg33,  const kernelArg &arg34,  const kernelArg &arg35, 
                      const kernelArg &arg36,  const kernelArg &arg37,  const kernelArg &arg38, 
                      const kernelArg &arg39,  const kernelArg &arg40,  const kernelArg &arg41, 
                      const kernelArg &arg42,  const kernelArg &arg43){
    SerialKernelData_t &data_ = *((SerialKernelData_t*) data);
    handleFunction_t tmpKernel = (handleFunction_t) data_.handle;
    int occaKernelArgs[6];

    occaKernelArgs[0] = outer.z;
    occaKernelArgs[1] = outer.y;
    occaKernelArgs[2] = outer.x;
    occaKernelArgs[3] = inner.z;
    occaKernelArgs[4] = inner.y;
    occaKernelArgs[5] = inner.x;

    int occaInnerId0 = 0, occaInnerId1 = 0, occaInnerId2 = 0;

    tmpKernel(occaKernelArgs,
              occaInnerId0, occaInnerId1, occaInnerId2,
              arg0.data(),
              arg1.data(),
              arg2.data(),
              arg3.data(),
              arg4.data(),
              arg5.data(),
              arg6.data(),
              arg7.data(),
              arg8.data(),
              arg9.data(),
              arg10.data(),
              arg11.data(),
              arg12.data(),
              arg13.data(),
              arg14.data(),
              arg15.data(),
              arg16.data(),
              arg17.data(),
              arg18.data(),
              arg19.data(),
              arg20.data(),
              arg21.data(),
              arg22.data(),
              arg23.data(),
              arg24.data(),
              arg25.data(),
              arg26.data(),
              arg27.data(),
              arg28.data(),
              arg29.data(),
              arg30.data(),
              arg31.data(),
              arg32.data(),
              arg33.data(),
              arg34.data(),
              arg35.data(),
              arg36.data(),
              arg37.data(),
              arg38.data(),
              arg39.data(),
              arg40.data(),
              arg41.data(),
              arg42.data(),
              arg43.data());
  }

  template <>
  void kernel_t<Serial>::operator () (const kernelArg &arg0,  const kernelArg &arg1,  const kernelArg &arg2, 
                      const kernelArg &arg3,  const kernelArg &arg4,  const kernelArg &arg5, 
                      const kernelArg &arg6,  const kernelArg &arg7,  const kernelArg &arg8, 
                      const kernelArg &arg9,  const kernelArg &arg10,  const kernelArg &arg11, 
                      const kernelArg &arg12,  const kernelArg &arg13,  const kernelArg &arg14, 
                      const kernelArg &arg15,  const kernelArg &arg16,  const kernelArg &arg17, 
                      const kernelArg &arg18,  const kernelArg &arg19,  const kernelArg &arg20, 
                      const kernelArg &arg21,  const kernelArg &arg22,  const kernelArg &arg23, 
                      const kernelArg &arg24,  const kernelArg &arg25,  const kernelArg &arg26, 
                      const kernelArg &arg27,  const kernelArg &arg28,  const kernelArg &arg29, 
                      const kernelArg &arg30,  const kernelArg &arg31,  const kernelArg &arg32, 
                      const kernelArg &arg33,  const kernelArg &arg34,  const kernelArg &arg35, 
                      const kernelArg &arg36,  const kernelArg &arg37,  const kernelArg &arg38, 
                      const kernelArg &arg39,  const kernelArg &arg40,  const kernelArg &arg41, 
                      const kernelArg &arg42,  const kernelArg &arg43,  const kernelArg &arg44){
    SerialKernelData_t &data_ = *((SerialKernelData_t*) data);
    handleFunction_t tmpKernel = (handleFunction_t) data_.handle;
    int occaKernelArgs[6];

    occaKernelArgs[0] = outer.z;
    occaKernelArgs[1] = outer.y;
    occaKernelArgs[2] = outer.x;
    occaKernelArgs[3] = inner.z;
    occaKernelArgs[4] = inner.y;
    occaKernelArgs[5] = inner.x;

    int occaInnerId0 = 0, occaInnerId1 = 0, occaInnerId2 = 0;

    tmpKernel(occaKernelArgs,
              occaInnerId0, occaInnerId1, occaInnerId2,
              arg0.data(),
              arg1.data(),
              arg2.data(),
              arg3.data(),
              arg4.data(),
              arg5.data(),
              arg6.data(),
              arg7.data(),
              arg8.data(),
              arg9.data(),
              arg10.data(),
              arg11.data(),
              arg12.data(),
              arg13.data(),
              arg14.data(),
              arg15.data(),
              arg16.data(),
              arg17.data(),
              arg18.data(),
              arg19.data(),
              arg20.data(),
              arg21.data(),
              arg22.data(),
              arg23.data(),
              arg24.data(),
              arg25.data(),
              arg26.data(),
              arg27.data(),
              arg28.data(),
              arg29.data(),
              arg30.data(),
              arg31.data(),
              arg32.data(),
              arg33.data(),
              arg34.data(),
              arg35.data(),
              arg36.data(),
              arg37.data(),
              arg38.data(),
              arg39.data(),
              arg40.data(),
              arg41.data(),
              arg42.data(),
              arg43.data(),
              arg44.data());
  }

  template <>
  void kernel_t<Serial>::operator () (const kernelArg &arg0,  const kernelArg &arg1,  const kernelArg &arg2, 
                      const kernelArg &arg3,  const kernelArg &arg4,  const kernelArg &arg5, 
                      const kernelArg &arg6,  const kernelArg &arg7,  const kernelArg &arg8, 
                      const kernelArg &arg9,  const kernelArg &arg10,  const kernelArg &arg11, 
                      const kernelArg &arg12,  const kernelArg &arg13,  const kernelArg &arg14, 
                      const kernelArg &arg15,  const kernelArg &arg16,  const kernelArg &arg17, 
                      const kernelArg &arg18,  const kernelArg &arg19,  const kernelArg &arg20, 
                      const kernelArg &arg21,  const kernelArg &arg22,  const kernelArg &arg23, 
                      const kernelArg &arg24,  const kernelArg &arg25,  const kernelArg &arg26, 
                      const kernelArg &arg27,  const kernelArg &arg28,  const kernelArg &arg29, 
                      const kernelArg &arg30,  const kernelArg &arg31,  const kernelArg &arg32, 
                      const kernelArg &arg33,  const kernelArg &arg34,  const kernelArg &arg35, 
                      const kernelArg &arg36,  const kernelArg &arg37,  const kernelArg &arg38, 
                      const kernelArg &arg39,  const kernelArg &arg40,  const kernelArg &arg41, 
                      const kernelArg &arg42,  const kernelArg &arg43,  const kernelArg &arg44, 
                      const kernelArg &arg45){
    SerialKernelData_t &data_ = *((SerialKernelData_t*) data);
    handleFunction_t tmpKernel = (handleFunction_t) data_.handle;
    int occaKernelArgs[6];

    occaKernelArgs[0] = outer.z;
    occaKernelArgs[1] = outer.y;
    occaKernelArgs[2] = outer.x;
    occaKernelArgs[3] = inner.z;
    occaKernelArgs[4] = inner.y;
    occaKernelArgs[5] = inner.x;

    int occaInnerId0 = 0, occaInnerId1 = 0, occaInnerId2 = 0;

    tmpKernel(occaKernelArgs,
              occaInnerId0, occaInnerId1, occaInnerId2,
              arg0.data(),
              arg1.data(),
              arg2.data(),
              arg3.data(),
              arg4.data(),
              arg5.data(),
              arg6.data(),
              arg7.data(),
              arg8.data(),
              arg9.data(),
              arg10.data(),
              arg11.data(),
              arg12.data(),
              arg13.data(),
              arg14.data(),
              arg15.data(),
              arg16.data(),
              arg17.data(),
              arg18.data(),
              arg19.data(),
              arg20.data(),
              arg21.data(),
              arg22.data(),
              arg23.data(),
              arg24.data(),
              arg25.data(),
              arg26.data(),
              arg27.data(),
              arg28.data(),
              arg29.data(),
              arg30.data(),
              arg31.data(),
              arg32.data(),
              arg33.data(),
              arg34.data(),
              arg35.data(),
              arg36.data(),
              arg37.data(),
              arg38.data(),
              arg39.data(),
              arg40.data(),
              arg41.data(),
              arg42.data(),
              arg43.data(),
              arg44.data(),
              arg45.data());
  }

  template <>
  void kernel_t<Serial>::operator () (const kernelArg &arg0,  const kernelArg &arg1,  const kernelArg &arg2, 
                      const kernelArg &arg3,  const kernelArg &arg4,  const kernelArg &arg5, 
                      const kernelArg &arg6,  const kernelArg &arg7,  const kernelArg &arg8, 
                      const kernelArg &arg9,  const kernelArg &arg10,  const kernelArg &arg11, 
                      const kernelArg &arg12,  const kernelArg &arg13,  const kernelArg &arg14, 
                      const kernelArg &arg15,  const kernelArg &arg16,  const kernelArg &arg17, 
                      const kernelArg &arg18,  const kernelArg &arg19,  const kernelArg &arg20, 
                      const kernelArg &arg21,  const kernelArg &arg22,  const kernelArg &arg23, 
                      const kernelArg &arg24,  const kernelArg &arg25,  const kernelArg &arg26, 
                      const kernelArg &arg27,  const kernelArg &arg28,  const kernelArg &arg29, 
                      const kernelArg &arg30,  const kernelArg &arg31,  const kernelArg &arg32, 
                      const kernelArg &arg33,  const kernelArg &arg34,  const kernelArg &arg35, 
                      const kernelArg &arg36,  const kernelArg &arg37,  const kernelArg &arg38, 
                      const kernelArg &arg39,  const kernelArg &arg40,  const kernelArg &arg41, 
                      const kernelArg &arg42,  const kernelArg &arg43,  const kernelArg &arg44, 
                      const kernelArg &arg45,  const kernelArg &arg46){
    SerialKernelData_t &data_ = *((SerialKernelData_t*) data);
    handleFunction_t tmpKernel = (handleFunction_t) data_.handle;
    int occaKernelArgs[6];

    occaKernelArgs[0] = outer.z;
    occaKernelArgs[1] = outer.y;
    occaKernelArgs[2] = outer.x;
    occaKernelArgs[3] = inner.z;
    occaKernelArgs[4] = inner.y;
    occaKernelArgs[5] = inner.x;

    int occaInnerId0 = 0, occaInnerId1 = 0, occaInnerId2 = 0;

    tmpKernel(occaKernelArgs,
              occaInnerId0, occaInnerId1, occaInnerId2,
              arg0.data(),
              arg1.data(),
              arg2.data(),
              arg3.data(),
              arg4.data(),
              arg5.data(),
              arg6.data(),
              arg7.data(),
              arg8.data(),
              arg9.data(),
              arg10.data(),
              arg11.data(),
              arg12.data(),
              arg13.data(),
              arg14.data(),
              arg15.data(),
              arg16.data(),
              arg17.data(),
              arg18.data(),
              arg19.data(),
              arg20.data(),
              arg21.data(),
              arg22.data(),
              arg23.data(),
              arg24.data(),
              arg25.data(),
              arg26.data(),
              arg27.data(),
              arg28.data(),
              arg29.data(),
              arg30.data(),
              arg31.data(),
              arg32.data(),
              arg33.data(),
              arg34.data(),
              arg35.data(),
              arg36.data(),
              arg37.data(),
              arg38.data(),
              arg39.data(),
              arg40.data(),
              arg41.data(),
              arg42.data(),
              arg43.data(),
              arg44.data(),
              arg45.data(),
              arg46.data());
  }

  template <>
  void kernel_t<Serial>::operator () (const kernelArg &arg0,  const kernelArg &arg1,  const kernelArg &arg2, 
                      const kernelArg &arg3,  const kernelArg &arg4,  const kernelArg &arg5, 
                      const kernelArg &arg6,  const kernelArg &arg7,  const kernelArg &arg8, 
                      const kernelArg &arg9,  const kernelArg &arg10,  const kernelArg &arg11, 
                      const kernelArg &arg12,  const kernelArg &arg13,  const kernelArg &arg14, 
                      const kernelArg &arg15,  const kernelArg &arg16,  const kernelArg &arg17, 
                      const kernelArg &arg18,  const kernelArg &arg19,  const kernelArg &arg20, 
                      const kernelArg &arg21,  const kernelArg &arg22,  const kernelArg &arg23, 
                      const kernelArg &arg24,  const kernelArg &arg25,  const kernelArg &arg26, 
                      const kernelArg &arg27,  const kernelArg &arg28,  const kernelArg &arg29, 
                      const kernelArg &arg30,  const kernelArg &arg31,  const kernelArg &arg32, 
                      const kernelArg &arg33,  const kernelArg &arg34,  const kernelArg &arg35, 
                      const kernelArg &arg36,  const kernelArg &arg37,  const kernelArg &arg38, 
                      const kernelArg &arg39,  const kernelArg &arg40,  const kernelArg &arg41, 
                      const kernelArg &arg42,  const kernelArg &arg43,  const kernelArg &arg44, 
                      const kernelArg &arg45,  const kernelArg &arg46,  const kernelArg &arg47){
    SerialKernelData_t &data_ = *((SerialKernelData_t*) data);
    handleFunction_t tmpKernel = (handleFunction_t) data_.handle;
    int occaKernelArgs[6];

    occaKernelArgs[0] = outer.z;
    occaKernelArgs[1] = outer.y;
    occaKernelArgs[2] = outer.x;
    occaKernelArgs[3] = inner.z;
    occaKernelArgs[4] = inner.y;
    occaKernelArgs[5] = inner.x;

    int occaInnerId0 = 0, occaInnerId1 = 0, occaInnerId2 = 0;

    tmpKernel(occaKernelArgs,
              occaInnerId0, occaInnerId1, occaInnerId2,
              arg0.data(),
              arg1.data(),
              arg2.data(),
              arg3.data(),
              arg4.data(),
              arg5.data(),
              arg6.data(),
              arg7.data(),
              arg8.data(),
              arg9.data(),
              arg10.data(),
              arg11.data(),
              arg12.data(),
              arg13.data(),
              arg14.data(),
              arg15.data(),
              arg16.data(),
              arg17.data(),
              arg18.data(),
              arg19.data(),
              arg20.data(),
              arg21.data(),
              arg22.data(),
              arg23.data(),
              arg24.data(),
              arg25.data(),
              arg26.data(),
              arg27.data(),
              arg28.data(),
              arg29.data(),
              arg30.data(),
              arg31.data(),
              arg32.data(),
              arg33.data(),
              arg34.data(),
              arg35.data(),
              arg36.data(),
              arg37.data(),
              arg38.data(),
              arg39.data(),
              arg40.data(),
              arg41.data(),
              arg42.data(),
              arg43.data(),
              arg44.data(),
              arg45.data(),
              arg46.data(),
              arg47.data());
  }

  template <>
  void kernel_t<Serial>::operator () (const kernelArg &arg0,  const kernelArg &arg1,  const kernelArg &arg2, 
                      const kernelArg &arg3,  const kernelArg &arg4,  const kernelArg &arg5, 
                      const kernelArg &arg6,  const kernelArg &arg7,  const kernelArg &arg8, 
                      const kernelArg &arg9,  const kernelArg &arg10,  const kernelArg &arg11, 
                      const kernelArg &arg12,  const kernelArg &arg13,  const kernelArg &arg14, 
                      const kernelArg &arg15,  const kernelArg &arg16,  const kernelArg &arg17, 
                      const kernelArg &arg18,  const kernelArg &arg19,  const kernelArg &arg20, 
                      const kernelArg &arg21,  const kernelArg &arg22,  const kernelArg &arg23, 
                      const kernelArg &arg24,  const kernelArg &arg25,  const kernelArg &arg26, 
                      const kernelArg &arg27,  const kernelArg &arg28,  const kernelArg &arg29, 
                      const kernelArg &arg30,  const kernelArg &arg31,  const kernelArg &arg32, 
                      const kernelArg &arg33,  const kernelArg &arg34,  const kernelArg &arg35, 
                      const kernelArg &arg36,  const kernelArg &arg37,  const kernelArg &arg38, 
                      const kernelArg &arg39,  const kernelArg &arg40,  const kernelArg &arg41, 
                      const kernelArg &arg42,  const kernelArg &arg43,  const kernelArg &arg44, 
                      const kernelArg &arg45,  const kernelArg &arg46,  const kernelArg &arg47, 
                      const kernelArg &arg48){
    SerialKernelData_t &data_ = *((SerialKernelData_t*) data);
    handleFunction_t tmpKernel = (handleFunction_t) data_.handle;
    int occaKernelArgs[6];

    occaKernelArgs[0] = outer.z;
    occaKernelArgs[1] = outer.y;
    occaKernelArgs[2] = outer.x;
    occaKernelArgs[3] = inner.z;
    occaKernelArgs[4] = inner.y;
    occaKernelArgs[5] = inner.x;

    int occaInnerId0 = 0, occaInnerId1 = 0, occaInnerId2 = 0;

    tmpKernel(occaKernelArgs,
              occaInnerId0, occaInnerId1, occaInnerId2,
              arg0.data(),
              arg1.data(),
              arg2.data(),
              arg3.data(),
              arg4.data(),
              arg5.data(),
              arg6.data(),
              arg7.data(),
              arg8.data(),
              arg9.data(),
              arg10.data(),
              arg11.data(),
              arg12.data(),
              arg13.data(),
              arg14.data(),
              arg15.data(),
              arg16.data(),
              arg17.data(),
              arg18.data(),
              arg19.data(),
              arg20.data(),
              arg21.data(),
              arg22.data(),
              arg23.data(),
              arg24.data(),
              arg25.data(),
              arg26.data(),
              arg27.data(),
              arg28.data(),
              arg29.data(),
              arg30.data(),
              arg31.data(),
              arg32.data(),
              arg33.data(),
              arg34.data(),
              arg35.data(),
              arg36.data(),
              arg37.data(),
              arg38.data(),
              arg39.data(),
              arg40.data(),
              arg41.data(),
              arg42.data(),
              arg43.data(),
              arg44.data(),
              arg45.data(),
              arg46.data(),
              arg47.data(),
              arg48.data());
  }

  template <>
  void kernel_t<Serial>::operator () (const kernelArg &arg0,  const kernelArg &arg1,  const kernelArg &arg2, 
                      const kernelArg &arg3,  const kernelArg &arg4,  const kernelArg &arg5, 
                      const kernelArg &arg6,  const kernelArg &arg7,  const kernelArg &arg8, 
                      const kernelArg &arg9,  const kernelArg &arg10,  const kernelArg &arg11, 
                      const kernelArg &arg12,  const kernelArg &arg13,  const kernelArg &arg14, 
                      const kernelArg &arg15,  const kernelArg &arg16,  const kernelArg &arg17, 
                      const kernelArg &arg18,  const kernelArg &arg19,  const kernelArg &arg20, 
                      const kernelArg &arg21,  const kernelArg &arg22,  const kernelArg &arg23, 
                      const kernelArg &arg24,  const kernelArg &arg25,  const kernelArg &arg26, 
                      const kernelArg &arg27,  const kernelArg &arg28,  const kernelArg &arg29, 
                      const kernelArg &arg30,  const kernelArg &arg31,  const kernelArg &arg32, 
                      const kernelArg &arg33,  const kernelArg &arg34,  const kernelArg &arg35, 
                      const kernelArg &arg36,  const kernelArg &arg37,  const kernelArg &arg38, 
                      const kernelArg &arg39,  const kernelArg &arg40,  const kernelArg &arg41, 
                      const kernelArg &arg42,  const kernelArg &arg43,  const kernelArg &arg44, 
                      const kernelArg &arg45,  const kernelArg &arg46,  const kernelArg &arg47, 
                      const kernelArg &arg48,  const kernelArg &arg49){
    SerialKernelData_t &data_ = *((SerialKernelData_t*) data);
    handleFunction_t tmpKernel = (handleFunction_t) data_.handle;
    int occaKernelArgs[6];

    occaKernelArgs[0] = outer.z;
    occaKernelArgs[1] = outer.y;
    occaKernelArgs[2] = outer.x;
    occaKernelArgs[3] = inner.z;
    occaKernelArgs[4] = inner.y;
    occaKernelArgs[5] = inner.x;

    int occaInnerId0 = 0, occaInnerId1 = 0, occaInnerId2 = 0;

    tmpKernel(occaKernelArgs,
              occaInnerId0, occaInnerId1, occaInnerId2,
              arg0.data(),
              arg1.data(),
              arg2.data(),
              arg3.data(),
              arg4.data(),
              arg5.data(),
              arg6.data(),
              arg7.data(),
              arg8.data(),
              arg9.data(),
              arg10.data(),
              arg11.data(),
              arg12.data(),
              arg13.data(),
              arg14.data(),
              arg15.data(),
              arg16.data(),
              arg17.data(),
              arg18.data(),
              arg19.data(),
              arg20.data(),
              arg21.data(),
              arg22.data(),
              arg23.data(),
              arg24.data(),
              arg25.data(),
              arg26.data(),
              arg27.data(),
              arg28.data(),
              arg29.data(),
              arg30.data(),
              arg31.data(),
              arg32.data(),
              arg33.data(),
              arg34.data(),
              arg35.data(),
              arg36.data(),
              arg37.data(),
              arg38.data(),
              arg39.data(),
              arg40.data(),
              arg41.data(),
              arg42.data(),
              arg43.data(),
              arg44.data(),
              arg45.data(),
              arg46.data(),
              arg47.data(),
              arg48.data(),
              arg49.data());
  }