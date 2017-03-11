/*
Increment a vector, one value per work item.

It is useless to do this on a GPU, not enough work / IO,
it's just a clEnqueueNDRangeKernel + get_global_id hello world.

- http://stackoverflow.com/questions/15194798/vector-step-addition-slower-on-cuda
- http://stackoverflow.com/questions/22005405/how-to-add-up-the-elements-of-an-array-in-gpu-any-function-similar-to-cublasdas
- http://stackoverflow.com/questions/15161575/reduction-for-sum-of-vector-when-size-is-not-power-of-2
*/

#include "common.h"

int main(void) {
    const char *source =
        "__kernel void main(__global int *out) {\n"
        "      out[get_global_id(0)]++;\n"
        "}\n";
    cl_command_queue command_queue;
    cl_int input[] = {1, 2};
    cl_mem buffer;
    const size_t global_work_size = sizeof(input) / sizeof(cl_int);
    Common common;

	/* Run kernel. */
    common_init(&common, source);
    buffer = clCreateBuffer(common.context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(input), &input, NULL);
    clSetKernelArg(common.kernel, 0, sizeof(cl_mem), &buffer);
    command_queue = clCreateCommandQueue(common.context, common.device, 0, NULL);
    clEnqueueNDRangeKernel(command_queue, common.kernel, 1, NULL, &global_work_size, NULL, 0, NULL, NULL);
    clFlush(command_queue);
    clFinish(command_queue);
    clEnqueueReadBuffer(command_queue, buffer, CL_TRUE, 0, sizeof(input), &input, 0, NULL, NULL);

	/* Assertions. */
    assert(input[0] == 2);
    assert(input[1] == 3);

	/* Cleanup. */
    clReleaseMemObject(buffer);
    clReleaseCommandQueue(command_queue);
    common_deinit(&common);
    return EXIT_SUCCESS;
}
