#!/bin/bash

echo_sucess() {
    echo -e "\033[1;32m $1 \033[0m"
}
echo_error() {
    echo -e "\033[1;31m $1 \033[0m"
}
echo_warning() {
    echo -e "\033[1;33m $1 \033[0m"
}
echo_info() {
    echo -e "\033[1;34m $1 \033[0m"
}

# Absolute path of working environment
if [ $1 ]; then
    compiled_model=$1
    echo_info "compiled_model: "$1
else
    compiled_model=../compiled_model/test_net
    echo_warning "compiled_model: "$compiled_model
fi

# Executable program of plate end.
model_dir=../model
tool_bin2header=./bin2header
chmod 0777 $tool_bin2header
model_r_cfg=$(find $compiled_model -maxdepth 1 -name "*_r.cfg")
model_r_weight=$(find $compiled_model -maxdepth 1 -name "*_r.weight")
model_input=$(find $compiled_model -maxdepth 1 -name "*.bin")

if [ ! -z "$model_r_cfg" ] && [ -f $model_r_cfg ];then
    $tool_bin2header $model_r_cfg $model_dir/model_cfg.h
else
    echo_error "can't find *_r.cfg"
    exit 1
fi

if [ ! -z "$model_r_weight" ] && [ -f $model_r_weight ];then
    $tool_bin2header $model_r_weight $model_dir/model_weight.h
else
    echo_error "can't find *_r.weight"
    exit 1
fi

if [ ! -z "$model_input" ] && [ -f $model_input ];then
    $tool_bin2header $model_input $model_dir/model_input.h
else
    model_input=$(find $compiled_model -maxdepth 1 -name "*.input")
    if [ ! -z "$model_input" ] && [ -f $model_input ];then
        $tool_bin2header $model_input $model_dir/model_input.h
    else
        model_input=$(find $compiled_model -maxdepth 1 -name "*.data")
        if [ ! -z "$model_input" ] && [ -f $model_input ];then
            $tool_bin2header $model_input $model_dir/model_input.h
        else
            echo_error "can't find *.bin or *.input or *.data"
            exit 1
        fi
    fi
fi

cd ../simple_forward_demo
make clean;make run
result=$?
libs_outpath_simple_forward_demo=build_sim/Release/simple_forward_demo.elf
if [ -f $libs_outpath_simple_forward_demo ] && [ $result == 0 ]; then
    echo_sucess "==========Runtime model: "${model_name}" SUCCESS!=========="
else
    echo_error "==========Runtime model: "${model_name}" FAILED!=========="
    exit 1
fi
