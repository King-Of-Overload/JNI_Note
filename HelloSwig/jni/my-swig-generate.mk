#
#@author AlanCroft
#
#检查变量MY_SWIG_PACKAGE是否已经定义
ifndef MY_SWIG_PACKAGE
$(error MY_SWIG_PACAGE is not defined.)
endif
#用斜杠替换java目录中的圆点
MY_SWIG_OUTDIR:=$(NDK_PROJECT_PATH)/src/$(subst .,/,$(MY_SWIG_PACKAGE))
 
#设置SWIG默认模式为C
ifndef MY_SWIG_TYPE
MY_SWIG_TYPE:=c
endif

#设置编译模式
ifeq ($(MY_SWIG_TYPE),cxx)
MY_SWIG_MODE:=-c++
else
MY_SWIG_MODE:=
endif
#追加SWIG封装源文件
LOCAL_SRC_FILES+= $(foreach MY_SWIG_INTERFACE,\
$(MY_SWIG_INTERFACES),\
$(basename $(MY_SWIG_INTERFACE)))_wrap.$(MY_SWIG_TYPE)
#添加.cxx作为c++文件扩展名
LOCAL_CPP_EXTENISON+=.cxx
#生成SWIG封代码
%_wrap.$(MY_SWIG_TYPE):%.i
	$(call host-mkdir,$(MY_SWIG_OUTDIR))
	swig -java \
	$(MY_SWIG_MODE) \
	-package $(MY_SWIG_PACKAGE) \
	-outdir $(MY_SWIG_OUTDIR) \
	$<