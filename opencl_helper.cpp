#include "opencl_helper.hpp"


std::ostream& operator<<(std::ostream& os, const OpenCLException& exception)
{
	if(exception.errorDetail.empty())
	{
		os << "Function name: " << exception.errorFunction << ", Error: " << ShowError(exception.err) << "(" << exception.err << ")";
	}
	else
	{
		os << "Function name: " << exception.errorFunction << "(" << exception.errorDetail  << "), Error: " << ShowError(exception.err) << "(" << exception.err << ")";		

	}
	return os;
}

OpenCLController::OpenCLController(cl_device_type openclDeviceType)
{

	char deviceName[100] = {0};
	size_t resultLength;

	cl_int err;

	bool contextInitialized = false;
	bool queueInitalized = false;
	try
	{

		err = clGetPlatformIDs(PLATFORM_MAX, platforms, &nplatforms);
		if(err != CL_SUCCESS) throw OpenCLException(err, "clGetPlatformIDs");
		if(nplatforms == 0)
		{
			err = CL_INVALID_VALUE;
			std::cerr << "No platforms." << std::endl;
			throw OpenCLException(err, "clGetPlatformIDs");
		}

	
		for(int i = 0;i < nplatforms;i++)
		{
			char vendor[100] = {0};
			char version[100] = {0};
        
			err = clGetPlatformInfo(platforms[i], CL_PLATFORM_VENDOR, sizeof(vendor), vendor, 0);
			if(err != CL_SUCCESS) throw OpenCLException(err, "clGetPlatformInfo", "CL_PLATFORM_VENDOR");

			err = clGetPlatformInfo(platforms[i], CL_PLATFORM_VERSION, sizeof(version), version, 0);
			if(err != CL_SUCCESS) throw OpenCLException(err, "clGetPlatformInfo", "CL_PLATFORM_VERSION");
			std::cerr << "Platform id:" << platforms[i] << ", Vendor: " << vendor << ", Version: " << version << std::endl;
		}	


		//Get device(GPU) information
		err = clGetDeviceIDs(platforms[0], openclDeviceType, DEVICE_MAX, devices, &ndevices);
		if(err != CL_SUCCESS) throw OpenCLException(err, "clGetDeviceIDs");
		
		if(ndevices == 0)
		{

			err = CL_INVALID_DEVICE;
			std::cerr << "No devices." << std::endl;
			throw OpenCLException(err, "clGetPlatformIDs");

		}
    
		std::cerr << ndevices << " device(s) found, first device will be used." << std::endl;
		err = clGetDeviceInfo(devices[0], CL_DEVICE_NAME, sizeof(deviceName), deviceName, &resultLength);
		if(err != CL_SUCCESS) throw OpenCLException(err, "clGetDeviceInfo", "CL_DEVICE_NAME");
		
		std::cerr << "Device name: " << deviceName << std::endl;



		err = clGetDeviceInfo(devices[0], CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE, sizeof(cl_ulong), &constantMemSize, &resultLength);
		if(err != CL_SUCCESS) throw OpenCLException(err, "clGetDeviceInfo", "CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE");



		err = clGetDeviceInfo(devices[0], CL_DEVICE_HOST_UNIFIED_MEMORY, sizeof(cl_bool), &unifiedMemoryAvailable, &resultLength);
		if(err != CL_SUCCESS) throw OpenCLException(err, "clGetDeviceInfo", "CL_DEVICE_HOST_UNIFIED_MEMORY");

		
		context = clCreateContext(nullptr, 1, devices, nullptr, nullptr, &err);
		if(err != CL_SUCCESS)
		{
			throw OpenCLException(err, "clCreateContext");
		}
		contextInitialized = true;

		
		queue = clCreateCommandQueue(context, devices[0], 0, &err);

		if(err != CL_SUCCESS)
		{
			throw OpenCLException(err, "clCreateCommandQueue");
		}
		queueInitalized = true;
	}
	catch(OpenCLException exception)
	{
		if(queueInitalized)
		{
			clReleaseCommandQueue(queue);
		}

		if(contextInitialized)
		{
			clReleaseContext(context);
		}
		throw exception;
	}
	
}

OpenCLController::~OpenCLController()
{
	clReleaseCommandQueue(queue);
	clReleaseContext(context);
}


OpenCLKernel::OpenCLKernel(OpenCLProgram& program, std::string name)
	: program(program), name(name)
{
	try
	{
		cl_int err;
		kernel = clCreateKernel(OpenCLKernel::program.GetProgram(), name.c_str(), &err);
		if(err != CL_SUCCESS)
		{
			throw OpenCLException(err, "clCreateKernel", name.c_str());
		}		
	}
	catch(OpenCLException exception)
	{
		throw exception;
	}
}

OpenCLKernel::~OpenCLKernel()
{

	clReleaseKernel(kernel);

}

cl_event OpenCLKernel::Execute(cl_uint ndim, const std::vector<size_t> &globalWorkSize, const std::vector<size_t> &localWorkSize)
{
	cl_event event;

	try
	{
		if(ndim == 0 || 4 <= ndim)
		{
			throw OpenCLException(CL_INVALID_VALUE, "clEnqueueNDRangeKernel", name.c_str());
		}
		if(globalWorkSize.size() != ndim)
		{
			throw OpenCLException(CL_INVALID_VALUE, "clEnqueueNDRangeKernel", name.c_str());
		}
		if(localWorkSize.size() != ndim)
		{
			throw OpenCLException(CL_INVALID_VALUE, "clEnqueueNDRangeKernel", name.c_str());
		}
		
		cl_int err = clEnqueueNDRangeKernel(program.GetQueue(), kernel, ndim, nullptr, globalWorkSize.data(), localWorkSize.data(), 0, nullptr, &event);

		if(err != CL_SUCCESS)
		{
			throw OpenCLException(err, "clEnqueueNDRangeKernel", name.c_str());
		}
	}
	catch(OpenCLException exception)
	{
		throw exception;
	}

	
	return event;
}

cl_event OpenCLKernel::Execute(cl_uint ndim, const std::vector<size_t> &globalWorkSize)
{
	cl_event event;

	try
	{
		if(ndim == 0 || 4 <= ndim)
		{
			throw OpenCLException(CL_INVALID_VALUE, "clEnqueueNDRangeKernel", name.c_str());
		}
		if(globalWorkSize.size() != ndim)
		{
			throw OpenCLException(CL_INVALID_VALUE, "clEnqueueNDRangeKernel", name.c_str());
		}
		
		cl_int err = clEnqueueNDRangeKernel(program.GetQueue(), kernel, ndim, nullptr, globalWorkSize.data(), nullptr, 0, nullptr, &event);

		if(err != CL_SUCCESS)
		{
			throw OpenCLException(err, "clEnqueueNDRangeKernel", name.c_str());
		}
	}
	catch(OpenCLException exception)
	{
		throw exception;
	}

	
	return event;
}

OpenCLProgram::OpenCLProgram(OpenCLController& controller, std::string kernelFileName) :
	controller(controller)
{
	cl_int err;

	bool programInitialized = false;
	try
	{
		std::ifstream ifs(kernelFileName.c_str());
		ifs.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		std::istreambuf_iterator<char> kernelBeginItr(ifs);
		std::istreambuf_iterator<char> kernelEndItr;
		std::string kernelSource(kernelBeginItr, kernelEndItr);
		
		size_t kernelLength = kernelSource.size();
		const char* kernelSourcePointer  = kernelSource.c_str();

		program = clCreateProgramWithSource(controller.GetContext(), 1, &kernelSourcePointer, &kernelLength, &err);
		if(err != CL_SUCCESS) throw OpenCLException(err, "clCreateProgramWithSource");


		err = clBuildProgram(program, 1, controller.GetDevices(), nullptr, nullptr, nullptr);
		if(err != CL_SUCCESS) throw OpenCLException(err, "clCreateProgramWithSource");
	}
	catch(OpenCLException exception)
	{
		char log[1024];
		
		if(err == CL_BUILD_PROGRAM_FAILURE)
		{
			cl_int err = clGetProgramBuildInfo(program, *(controller.GetDevices()), CL_PROGRAM_BUILD_LOG, 1024, log, nullptr);
			if(err == CL_SUCCESS)
			{
				std::cerr << log << std::endl;
			}
			else
			{
				std::cerr << OpenCLException(err, "clGetProgramBuild", "CL_PROGRAM_BUILD_LOG") << std::endl;
			}
			
		}
		throw exception;
	}
	catch(std::ifstream::failure e)
	{
		throw OpenCLException(CL_INVALID_KERNEL, "OpenCLProgram::OpenCLProgram");
	}
	
}

OpenCLProgram::~OpenCLProgram()
{
			
	clReleaseProgram(program);

}


