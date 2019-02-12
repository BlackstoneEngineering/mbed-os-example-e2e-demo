// Auto generated by utensor-cli

#include "uTensor/ops/MathOps.hpp"
#include "uTensor/ops/ArrayOps.hpp"
#include "uTensor/ops/MatrixOps.hpp"
#include "iris_model_weight.hpp"
#include "iris_model.hpp"
#include "uTensor/core/context.hpp"
#include "uTensor/core/tensor.hpp"


void get_iris_model_ctx(Context& ctx, Tensor* input_0) {

{ // add tensor for placeholders
    ctx.add(input_0, "x:0", 2);
}
{    
    ctx.add(new BinaryTensor<int>({1}, inline_MatMul_eightbit_x__port__0_reshape_dims_0), 
            "MatMul_eightbit/x__port__0/reshape_dims:0", 
            1);
}
{
    ctx.add(new RamTensor<float>(), "MatMul_eightbit/x__port__0/reshape:0", 2);
    ctx.push(new ReshapeOp(), 
             { "x:0", "MatMul_eightbit/x__port__0/reshape_dims:0" },
             { "MatMul_eightbit/x__port__0/reshape:0" });
    ctx.eval();
}
{    
    ctx.add(new BinaryTensor<int>({1}, inline_MatMul_eightbit_x__port__0_reduction_dims_0), 
            "MatMul_eightbit/x__port__0/reduction_dims:0", 
            2);
}
{   
    RamTensor<float>* out_tensor;
    out_tensor = new RamTensor<float>({ 1 });
    ctx.add(out_tensor, "MatMul_eightbit/x__port__0/min:0", 1);
    ctx.push(new MinOp(), 
             { "MatMul_eightbit/x__port__0/reshape:0", "MatMul_eightbit/x__port__0/reduction_dims:0" },
             { "MatMul_eightbit/x__port__0/min:0" });
    ctx.eval();
}
{   
    RamTensor<float>* out_tensor;
    out_tensor = new RamTensor<float>({ 1 });
    ctx.add(out_tensor, "MatMul_eightbit/x__port__0/max:0", 1);
    ctx.push(new MaxOp(), 
             { "MatMul_eightbit/x__port__0/reshape:0", "MatMul_eightbit/x__port__0/reduction_dims:0" },
             { "MatMul_eightbit/x__port__0/max:0" });
    ctx.eval();
}
{
    ctx.add(new RamTensor<uint8_t>(), "MatMul_eightbit/x__port__0/quantize:0", 1);
    ctx.add(new RamTensor<float>({1}), "MatMul_eightbit/x__port__0/quantize:1", 1);
    ctx.add(new RamTensor<float>({1}), "MatMul_eightbit/x__port__0/quantize:2", 1);
    ctx.push(new QuantizeV2Op(),
             {  "x:0",  "MatMul_eightbit/x__port__0/min:0", "MatMul_eightbit/x__port__0/max:0" },
             {  "MatMul_eightbit/x__port__0/quantize:0",  "MatMul_eightbit/x__port__0/quantize:1", "MatMul_eightbit/x__port__0/quantize:2" });
    ctx.eval();
}
{    
    ctx.add(new BinaryTensor<float>({4,3}, inline_w_fc1_0), 
            "w_fc1:0", 
            2);
}
{    
    ctx.add(new BinaryTensor<int>({1}, inline_MatMul_eightbit_w_fc1__port__0_reshape_dims_0), 
            "MatMul_eightbit/w_fc1__port__0/reshape_dims:0", 
            1);
}
{
    ctx.add(new RamTensor<float>(), "MatMul_eightbit/w_fc1__port__0/reshape:0", 2);
    ctx.push(new ReshapeOp(), 
             { "w_fc1:0", "MatMul_eightbit/w_fc1__port__0/reshape_dims:0" },
             { "MatMul_eightbit/w_fc1__port__0/reshape:0" });
    ctx.eval();
}
{    
    ctx.add(new BinaryTensor<int>({1}, inline_MatMul_eightbit_w_fc1__port__0_reduction_dims_0), 
            "MatMul_eightbit/w_fc1__port__0/reduction_dims:0", 
            2);
}
{   
    RamTensor<float>* out_tensor;
    out_tensor = new RamTensor<float>({ 1 });
    ctx.add(out_tensor, "MatMul_eightbit/w_fc1__port__0/min:0", 1);
    ctx.push(new MinOp(), 
             { "MatMul_eightbit/w_fc1__port__0/reshape:0", "MatMul_eightbit/w_fc1__port__0/reduction_dims:0" },
             { "MatMul_eightbit/w_fc1__port__0/min:0" });
    ctx.eval();
}
{   
    RamTensor<float>* out_tensor;
    out_tensor = new RamTensor<float>({ 1 });
    ctx.add(out_tensor, "MatMul_eightbit/w_fc1__port__0/max:0", 1);
    ctx.push(new MaxOp(), 
             { "MatMul_eightbit/w_fc1__port__0/reshape:0", "MatMul_eightbit/w_fc1__port__0/reduction_dims:0" },
             { "MatMul_eightbit/w_fc1__port__0/max:0" });
    ctx.eval();
}
{
    ctx.add(new RamTensor<uint8_t>(), "MatMul_eightbit/w_fc1__port__0/quantize:0", 1);
    ctx.add(new RamTensor<float>({1}), "MatMul_eightbit/w_fc1__port__0/quantize:1", 1);
    ctx.add(new RamTensor<float>({1}), "MatMul_eightbit/w_fc1__port__0/quantize:2", 1);
    ctx.push(new QuantizeV2Op(),
             {  "w_fc1:0",  "MatMul_eightbit/w_fc1__port__0/min:0", "MatMul_eightbit/w_fc1__port__0/max:0" },
             {  "MatMul_eightbit/w_fc1__port__0/quantize:0",  "MatMul_eightbit/w_fc1__port__0/quantize:1", "MatMul_eightbit/w_fc1__port__0/quantize:2" });
    ctx.eval();
}
{
    ctx.add(new RamTensor<int>(), "MatMul/eightbit:0", 2);
    ctx.add(new RamTensor<float>({1}), "MatMul/eightbit:1", 2);
    ctx.add(new RamTensor<float>({1}), "MatMul/eightbit:2", 2);
    ctx.push(new QntMatMulOp<uint8_t, uint8_t, int>(), 
             { "MatMul_eightbit/x__port__0/quantize:0", "MatMul_eightbit/x__port__0/quantize:1", "MatMul_eightbit/x__port__0/quantize:2", "MatMul_eightbit/w_fc1__port__0/quantize:0", "MatMul_eightbit/w_fc1__port__0/quantize:1",  "MatMul_eightbit/w_fc1__port__0/quantize:2" },
             { "MatMul/eightbit:0", "MatMul/eightbit:1",  "MatMul/eightbit:2" });
    ctx.eval();
}
{
    ctx.add(new RamTensor<float>({1}), "MatMul/eightbit/requant_range:0", 1);
    ctx.add(new RamTensor<float>({1}), "MatMul/eightbit/requant_range:1", 1);
    ctx.push(new Requantization_RangeOp(),
             { "MatMul/eightbit:0", "MatMul/eightbit:1", "MatMul/eightbit:2" },
             { "MatMul/eightbit/requant_range:0", "MatMul/eightbit/requant_range:1" });
    ctx.eval();
}
{   
    ctx.add(new RamTensor<uint8_t>(), "MatMul/eightbit/requantize:0", 1);
    ctx.add(new RamTensor<float>({1}), "MatMul/eightbit/requantize:1", 1);
    ctx.add(new RamTensor<float>({1}), "MatMul/eightbit/requantize:2", 1);
    ctx.push(new RequantizeOp(),
             { "MatMul/eightbit:0", "MatMul/eightbit:1", "MatMul/eightbit:2", "MatMul/eightbit/requant_range:0", "MatMul/eightbit/requant_range:1" },
             { "MatMul/eightbit/requantize:0", "MatMul/eightbit/requantize:1", "MatMul/eightbit/requantize:2" });
    ctx.eval();
}
{    
    ctx.add(new BinaryTensor<float>({3}, inline_b_fc1_0), 
            "b_fc1:0", 
            2);
}
{    
    ctx.add(new BinaryTensor<int>({1}, inline_logits_eightbit_b_fc1__port__0_reshape_dims_0), 
            "logits_eightbit/b_fc1__port__0/reshape_dims:0", 
            1);
}
{
    ctx.add(new RamTensor<float>(), "logits_eightbit/b_fc1__port__0/reshape:0", 2);
    ctx.push(new ReshapeOp(), 
             { "b_fc1:0", "logits_eightbit/b_fc1__port__0/reshape_dims:0" },
             { "logits_eightbit/b_fc1__port__0/reshape:0" });
    ctx.eval();
}
{    
    ctx.add(new BinaryTensor<int>({1}, inline_logits_eightbit_b_fc1__port__0_reduction_dims_0), 
            "logits_eightbit/b_fc1__port__0/reduction_dims:0", 
            2);
}
{   
    RamTensor<float>* out_tensor;
    out_tensor = new RamTensor<float>({ 1 });
    ctx.add(out_tensor, "logits_eightbit/b_fc1__port__0/min:0", 1);
    ctx.push(new MinOp(), 
             { "logits_eightbit/b_fc1__port__0/reshape:0", "logits_eightbit/b_fc1__port__0/reduction_dims:0" },
             { "logits_eightbit/b_fc1__port__0/min:0" });
    ctx.eval();
}
{   
    RamTensor<float>* out_tensor;
    out_tensor = new RamTensor<float>({ 1 });
    ctx.add(out_tensor, "logits_eightbit/b_fc1__port__0/max:0", 1);
    ctx.push(new MaxOp(), 
             { "logits_eightbit/b_fc1__port__0/reshape:0", "logits_eightbit/b_fc1__port__0/reduction_dims:0" },
             { "logits_eightbit/b_fc1__port__0/max:0" });
    ctx.eval();
}
{
    ctx.add(new RamTensor<uint8_t>(), "logits_eightbit/b_fc1__port__0/quantize:0", 1);
    ctx.add(new RamTensor<float>({1}), "logits_eightbit/b_fc1__port__0/quantize:1", 1);
    ctx.add(new RamTensor<float>({1}), "logits_eightbit/b_fc1__port__0/quantize:2", 1);
    ctx.push(new QuantizeV2Op(),
             {  "b_fc1:0",  "logits_eightbit/b_fc1__port__0/min:0", "logits_eightbit/b_fc1__port__0/max:0" },
             {  "logits_eightbit/b_fc1__port__0/quantize:0",  "logits_eightbit/b_fc1__port__0/quantize:1", "logits_eightbit/b_fc1__port__0/quantize:2" });
    ctx.eval();
}
{
    ctx.add(new RamTensor<int>(), "logits/eightbit:0", 2);
    ctx.add(new RamTensor<float>({1}), "logits/eightbit:1", 2);
    ctx.add(new RamTensor<float>({1}), "logits/eightbit:2", 2);
    ctx.push(new QuantizedAddOp<uint8_t, uint8_t, int>(), 
             { "MatMul/eightbit/requantize:0", "MatMul/eightbit/requantize:1", "MatMul/eightbit/requantize:2", "logits_eightbit/b_fc1__port__0/quantize:0", "logits_eightbit/b_fc1__port__0/quantize:1",  "logits_eightbit/b_fc1__port__0/quantize:2" },
             { "logits/eightbit:0", "logits/eightbit:1",  "logits/eightbit:2" });
    ctx.eval();
}
{
    ctx.add(new RamTensor<float>({1}), "logits/eightbit/requant_range:0", 1);
    ctx.add(new RamTensor<float>({1}), "logits/eightbit/requant_range:1", 1);
    ctx.push(new Requantization_RangeOp(),
             { "logits/eightbit:0", "logits/eightbit:1", "logits/eightbit:2" },
             { "logits/eightbit/requant_range:0", "logits/eightbit/requant_range:1" });
    ctx.eval();
}
{   
    ctx.add(new RamTensor<uint8_t>(), "logits/eightbit/requantize:0", 1);
    ctx.add(new RamTensor<float>({1}), "logits/eightbit/requantize:1", 1);
    ctx.add(new RamTensor<float>({1}), "logits/eightbit/requantize:2", 1);
    ctx.push(new RequantizeOp(),
             { "logits/eightbit:0", "logits/eightbit:1", "logits/eightbit:2", "logits/eightbit/requant_range:0", "logits/eightbit/requant_range:1" },
             { "logits/eightbit/requantize:0", "logits/eightbit/requantize:1", "logits/eightbit/requantize:2" });
    ctx.eval();
}
{
    ctx.add(new RamTensor<float>(), "logits:0", 1);
    ctx.push(new DequantizeOp(), 
             { "logits/eightbit/requantize:0", "logits/eightbit/requantize:1", "logits/eightbit/requantize:2" },
             { "logits:0" });
    ctx.eval();
}
{    
    ctx.add(new BinaryTensor<int>({}, inline_y_pred_dimension_0), 
            "y_pred/dimension:0", 
            1);
}
{
    ctx.add(new RamTensor<int>(), "y_pred:0");
    ctx.push(new ArgMaxOp<float, int>(), 
             { "logits:0", "y_pred/dimension:0" },
             { "y_pred:0" });
}
}