#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <cassert>
#define CL_VERSION_1_1

//#include <cl.hpp>
#include <CL/cl.h>
//#include <CL/cl_platform.h>

std::string get_error_string(cl_int err){
     switch(err){
         case 0: return "CL_SUCCESS";
         case -1: return "CL_DEVICE_NOT_FOUND";
         case -2: return "CL_DEVICE_NOT_AVAILABLE";
         case -3: return "CL_COMPILER_NOT_AVAILABLE";
         case -4: return "CL_MEM_OBJECT_ALLOCATION_FAILURE";
         case -5: return "CL_OUT_OF_RESOURCES";
         case -6: return "CL_OUT_OF_HOST_MEMORY";
         case -7: return "CL_PROFILING_INFO_NOT_AVAILABLE";
         case -8: return "CL_MEM_COPY_OVERLAP";
         case -9: return "CL_IMAGE_FORMAT_MISMATCH";
         case -10: return "CL_IMAGE_FORMAT_NOT_SUPPORTED";
         case -11: return "CL_BUILD_PROGRAM_FAILURE";
         case -12: return "CL_MAP_FAILURE";

         case -30: return "CL_INVALID_VALUE";
         case -31: return "CL_INVALID_DEVICE_TYPE";
         case -32: return "CL_INVALID_PLATFORM";
         case -33: return "CL_INVALID_DEVICE";
         case -34: return "CL_INVALID_CONTEXT";
         case -35: return "CL_INVALID_QUEUE_PROPERTIES";
         case -36: return "CL_INVALID_COMMAND_QUEUE";
         case -37: return "CL_INVALID_HOST_PTR";
         case -38: return "CL_INVALID_MEM_OBJECT";
         case -39: return "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR";
         case -40: return "CL_INVALID_IMAGE_SIZE";
         case -41: return "CL_INVALID_SAMPLER";
         case -42: return "CL_INVALID_BINARY";
         case -43: return "CL_INVALID_BUILD_OPTIONS";
         case -44: return "CL_INVALID_PROGRAM";
         case -45: return "CL_INVALID_PROGRAM_EXECUTABLE";
         case -46: return "CL_INVALID_KERNEL_NAME";
         case -47: return "CL_INVALID_KERNEL_DEFINITION";
         case -48: return "CL_INVALID_KERNEL";
         case -49: return "CL_INVALID_ARG_INDEX";
         case -50: return "CL_INVALID_ARG_VALUE";
         case -51: return "CL_INVALID_ARG_SIZE";
         case -52: return "CL_INVALID_KERNEL_ARGS";
         case -53: return "CL_INVALID_WORK_DIMENSION";
         case -54: return "CL_INVALID_WORK_GROUP_SIZE";
         case -55: return "CL_INVALID_WORK_ITEM_SIZE";
         case -56: return "CL_INVALID_GLOBAL_OFFSET";
         case -57: return "CL_INVALID_EVENT_WAIT_LIST";
         case -58: return "CL_INVALID_EVENT";
         case -59: return "CL_INVALID_OPERATION";
         case -60: return "CL_INVALID_GL_OBJECT";
         case -61: return "CL_INVALID_BUFFER_SIZE";
         case -62: return "CL_INVALID_MIP_LEVEL";
         case -63: return "CL_INVALID_GLOBAL_WORK_SIZE";
         default: return "Unknown OpenCL error";
     }
 }
void CheckErrorAt(cl_int err,const char * source_info){
    if (err){
        std::cout << "Error: at " << source_info << ":\n" << get_error_string(err) << std::endl;
        exit(err);
    }
}
#define STR_HELPER(x) #x
#define CONST_STR(x) STR_HELPER(x)
#define CheckError(err) {CheckErrorAt(err,("File: " __FILE__ ", Line: " CONST_STR(__LINE__)));}

template<typename item_ty>
class CLBuffer{
protected:
    int bufsize;
    cl_mem buf;
    cl_context mycontext;
    cl_command_queue myqueue;
public:
    CLBuffer(){}
    CLBuffer(cl_context context,cl_command_queue queue,size_t size){
        bufsize = size;
        mycontext = context;
        myqueue = queue;

        cl_int error = CL_SUCCESS;
        buf = clCreateBuffer(context,CL_MEM_READ_WRITE,bytes(),nullptr,&error);
        CheckError(error);
        zero_buffer();
    }
    void zero_buffer(){
        item_ty zero;
        CheckError(clEnqueueFillBuffer(myqueue,
            buf,
            &zero,
            sizeof(zero),
            0,
            sizeof(zero)*bufsize,
            0,nullptr,
            nullptr
        ));
        CheckError(clEnqueueBarrier(myqueue));
    }

    void write_buffer(std::vector<item_ty>& data){
        write_buffer(data.data(), data.size());
    }
    void write_buffer(item_ty * data, size_t num_elmts){
        assert(num_elmts == bufsize);
        CheckError(clEnqueueWriteBuffer(myqueue,
                             buf,
                             CL_TRUE,
                             0,bufsize*sizeof(item_ty),
                             data,
                             0,nullptr,
                             nullptr));
    }
    void write_buffer(item_ty * data, size_t num_elmts, int offset){
        assert(num_elmts+offset <= bufsize);
        CheckError(clEnqueueWriteBuffer(myqueue,
                             buf,
                             CL_TRUE,
                             offset,bufsize*sizeof(item_ty),
                             data,
                             0,nullptr,
                             nullptr));
    }
    void read_buffer(std::vector<item_ty> & read_into){
        read_buffer(read_into.data(), read_into.size());
    }
    void read_buffer(item_ty * data, size_t num_elmts){
        // assert(num_elmts == bufsize);
        CheckError(clEnqueueReadBuffer(myqueue,
                             buf,
                             CL_TRUE,
                             0,num_elmts*sizeof(item_ty),
                             data,
                             0,nullptr,
                             nullptr));
    }
    cl_mem k_arg(){
        return buf;
    }
    size_t bytes(){
        return bufsize * sizeof(item_ty);
    }
    void copy_buffer(CLBuffer<item_ty> src_buf){
        assert(src_buf.bufsize == this->bufsize);
        assert(src_buf.myqueue == this->myqueue);
        assert(src_buf.mycontext == this->mycontext);
        CheckError(clEnqueueBarrier(myqueue));
        CheckError(clEnqueueCopyBuffer(myqueue,
                            src_buf.buf,this->buf,
                            0,0,
                            bytes(),
                            0,nullptr,
                            nullptr));
    }
};
class CL_NDRange{
public:
    size_t x;
    size_t y;
    size_t z;
    cl_uint ndim;
    CL_NDRange(size_t in_x,size_t in_y, size_t in_z){
        x = in_x;
        y = in_y;
        z = in_z;
        ndim = 3;
        //assert(in_x != 0);
        //assert(in_y != 0);
        //assert(in_z != 0);
    }
    CL_NDRange(size_t in_x,size_t in_y){
        x = in_x;
        y = in_y;
        z = -1;
        ndim = 2;
        //assert(in_x != 0);
        //assert(in_y != 0);
    }
    CL_NDRange(size_t in_x){
        x = in_x;
        y = -1;
        z = -1;
        ndim = 1;
        //assert(in_x != 0);
    }
    CL_NDRange(){
        x = -1;
        y = -1;
        z = -1;
        ndim = 0;
    }
    size_t * array_view(){
        return ndim == 0 ? nullptr : reinterpret_cast<size_t*>(this);
    }
    cl_uint dim(){
        return ndim;
    }
};
CL_NDRange div_nd(CL_NDRange range,CL_NDRange divisor){
    size_t * arr = range.array_view();
    size_t * div = divisor.array_view();
    for(int i = 0; i < 3; i++){
        arr[i] /= div[i];
    }
    return range;
}
class CLKernel{
protected:
    cl_command_queue myqueue;
    cl_program program;
    cl_kernel kern;
    CL_NDRange run_range;
    CL_NDRange group_range;
    CL_NDRange exec_range;
public:
    CLKernel(cl_program in_prog,cl_command_queue in_queue,const char * kern_name,CL_NDRange in_run_range,CL_NDRange in_group_range,CL_NDRange in_exec_range,std::vector<cl_mem> args){
        myqueue = in_queue;
        program = in_prog;
        run_range = in_run_range;
        group_range = in_group_range;
        exec_range = in_exec_range;

        assert(run_range.dim() > 0 && "run_range needs to have at least 1 dimention specified");

        // http://www.khronos.org/registry/cl/sdk/1.1/docs/man/xhtml/clCreateKernel.html
        cl_int error = CL_SUCCESS;
        kern = clCreateKernel (program, kern_name, &error);
        CheckError (error);

        int idx = 0;
        using namespace std;
        for(cl_mem b_info: args){
            CheckError(clSetKernelArg(kern,idx,sizeof(b_info),&b_info));
            idx++;
        }
    }
    void run(){
        CheckError(clEnqueueBarrier(myqueue));
        int dim = exec_range.dim();
        CL_NDRange exec_range = dim == 0 ? CL_NDRange(1,1,1) : this->exec_range;
        CL_NDRange glob_range = div_nd(this->run_range,exec_range);
        for(int x = 0; x < exec_range.x; x++){
            for(int y = 0; y < exec_range.y; y++){
                for(int z = 0; z < exec_range.z; z++){
                    CL_NDRange global_offset(x*glob_range.x,y*glob_range.y,z*glob_range.z);
                    CheckError(clEnqueueNDRangeKernel(
                                   myqueue,
                                   kern,
                                   run_range.dim(),
                                   global_offset.array_view(),
                                   glob_range.array_view(),
                                   group_range.array_view(),
                                   0,nullptr,
                                   nullptr
                                   ));
               }
           }
        }
    }
    void barrier(){
        CheckError(clEnqueueBarrier(myqueue));
    }
};


class OpenCLExecutor{
protected:
    std::string source_path;
    cl_platform_id platform;
    cl_device_id device;
    cl_context context;
    cl_program program;
    cl_command_queue queue;
    int platform_id;
    int device_id;
public:

    OpenCLExecutor(std::string in_source_path, int platform=0, int device=0)
    {
        source_path = in_source_path;
        platform_id = platform;
        device_id = device;
        build_program();
        std::cout << "finished building program" << std::endl;
    }
    ~OpenCLExecutor(){
        clReleaseProgram(program);
        clReleaseContext(context);
        clReleaseCommandQueue(queue);
    }
    template<typename item_ty>
    CLBuffer<item_ty> new_clbuffer(size_t size){
        return CLBuffer<item_ty>(context,queue,size);
    }
    CLKernel new_clkernel(const char * kern_name,CL_NDRange run_range,CL_NDRange in_group_range,CL_NDRange in_exec_range,std::vector<cl_mem> buflist){
        return CLKernel(program,queue,kern_name,run_range,in_group_range,in_exec_range,buflist);
    }

protected:
    void get_main_device(){
        // http://www.khronos.org/registry/cl/sdk/1.1/docs/man/xhtml/clGetPlatformIDs.html
        cl_uint platformIdCount = 0;
        clGetPlatformIDs (0, nullptr, &platformIdCount);

        if (platformIdCount == 0) {
            std::cerr << "No OpenCL platform found" << std::endl;
            exit(1);
        } else {
            std::cout << "Found " << platformIdCount << " platform(s)" << std::endl;
        }

        std::vector<cl_platform_id> platformIds (platformIdCount);
        clGetPlatformIDs (platformIdCount, platformIds.data (), nullptr);

        for (cl_uint i = 0; i < platformIdCount; ++i) {
            std::cout << "\t (" << (i+1) << ") : " << GetPlatformName (platformIds [i]) << std::endl;
        }

        // http://www.khronos.org/registry/cl/sdk/1.1/docs/man/xhtml/clGetDeviceIDs.html
        cl_uint deviceIdCount = 0;
        clGetDeviceIDs (platformIds [platform_id], CL_DEVICE_TYPE_ALL, 0, nullptr,
            &deviceIdCount);

        if (deviceIdCount == 0) {
            std::cerr << "No OpenCL devices found" << std::endl;
            exit(1);
        } else {
            std::cout << "Found " << deviceIdCount << " device(s)" << std::endl;
        }

        std::vector<cl_device_id> deviceIds (deviceIdCount);
        clGetDeviceIDs (platformIds [platform_id], CL_DEVICE_TYPE_ALL, deviceIdCount,
            deviceIds.data (), nullptr);

        for (cl_uint i = 0; i < deviceIdCount; ++i) {
            std::cout << "\t (" << (i+1) << ") : " << GetDeviceName (deviceIds [i]) << std::endl;
        }

        std::cout << "Using platform: "<< GetPlatformName (platformIds [platform_id])<<"\n";
        std::cout << "Using device: "<< GetDeviceName (deviceIds [device_id])<<"\n";

        this->platform = platformIds [platform_id];
        this->device = deviceIds[device_id];
    }
    void create_context(){
        // http://www.khronos.org/registry/cl/sdk/1.1/docs/man/xhtml/clCreateContext.html
        const cl_context_properties contextProperties [] =
        {
            CL_CONTEXT_PLATFORM, reinterpret_cast<cl_context_properties> (this->platform),
            0, 0
        };

        cl_int error = CL_SUCCESS;
        this->context = clCreateContext (contextProperties, 1,
            &device, nullptr, nullptr, &error);
        CheckError(error);
        std::cout << "Context created" << std::endl;
    }
    void create_queue(){
        // http://www.khronos.org/registry/cl/sdk/1.1/docs/man/xhtml/clCreateCommandQueue.html

        cl_int error = CL_SUCCESS;
        this->queue = clCreateCommandQueue (context, this->device,
            0, &error);
        CheckError (error);
    }
    void build_program(){
        get_main_device();
        create_context();
        create_queue();
        CreateProgram();
    }

    std::string get_source(){
        std::ifstream file(source_path);
        if(!file){
            std::cout << "the file " << source_path << " is missing!\n";
            exit(1);
        }
        //slow way to read a file (but file size is small)
        std::string fstr((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

        file.close();
        return fstr;
    }

    void CreateProgram ()
    {
        std::string source = get_source();
        // http://www.khronos.org/registry/cl/sdk/1.1/docs/man/xhtml/clCreateProgramWithSource.html
        size_t lengths [1] = { source.size () };
        const char* sources [1] = { source.data () };

        cl_int error = CL_SUCCESS;
        this->program = clCreateProgramWithSource (this->context, 1, sources, lengths, &error);
        CheckError (error);

        cl_int build_error = clBuildProgram (program, 1, &this->device,
            "", nullptr, nullptr);

        if(build_error == CL_BUILD_PROGRAM_FAILURE){
            size_t len = 0;
            cl_int ret = CL_SUCCESS;
            CheckError(clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &len));
            std::vector<char> data(len);
            CheckError(clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, len, data.data(), NULL));
            std::cout << "Build error:\n" << std::string(data.begin(),data.end()) << std::endl;
            exit(1);
        }
        else{
            CheckError(build_error);
        }
        //ret = clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, 0, NULL, &len);
    }

     std::string GetPlatformName (cl_platform_id id)
     {
         size_t size = 0;
         clGetPlatformInfo (id, CL_PLATFORM_NAME, 0, nullptr, &size);

         std::string result;
         result.resize (size);
         clGetPlatformInfo (id, CL_PLATFORM_NAME, size,
             const_cast<char*> (result.data ()), nullptr);

         return result;
     }

     std::string GetDeviceName (cl_device_id id)
     {
         size_t size = 0;
         clGetDeviceInfo (id, CL_DEVICE_NAME, 0, nullptr, &size);

         std::string result;
         result.resize (size);
         clGetDeviceInfo (id, CL_DEVICE_NAME, size,
             const_cast<char*> (result.data ()), nullptr);

         return result;
     }
};
