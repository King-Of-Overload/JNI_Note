  # 检查变量 MY_SWIG_PACKAGE是否定义 
    ifndef MY_SWIG_PACKAGE  
       $(error MY_SWIG_PACKAGE is not defined.)  
    endif  
     
    # 用斜线替换Java目录的圆点，指定swig的路径   
    MY_SWIG_OUTDIR:=$(NDK_PROJECT_PATH)/src/$(subst .,/,$(MY_SWIG_PACKAGE)) 
    #指定SWIG的默认类型是C 
    ifndef MY_SWIG_TYPE  
        MY_SWIG_TYPE:=c 
    endif  
     
    # 设置SWIG 的模式  
ifeq ($(MY_SWIG_TYPE),cxx)  
       MY_SWIG_MODE:= - c++  
    else  
       MY_SWIG_MODE:=    
    endif  
      
    # 追加SWIG封装源文件
LOCAL_SRC_FILES+= $(foreach MY_SWIG_INTERFACE,\
$(MY_SWIG_INTERFACES),\
$(basename $(MY_SWIG_INTERFACE))_wrap.$(MY_SWIG_TYPE))
     
    # 添加.cxx作为C++扩展名        
    LOCAL_CPP_EXTENSION+=.cxx  
     
    # 生成SWIG的封闭代码
%_warp.$(MY_SWIG_TYPE): %.i \
$(call host-mkdir,$(MY_SWIG_OUTDIR)) \
swig -java \
$(MY_SWIG_MODE) \
-package $(MY-SWIG_PACKAGE) \
-outdir $(MY_SWIG_OUTDIR) \
$<