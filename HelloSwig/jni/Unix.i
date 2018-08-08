%module(directors = 1) Unix

%{
#include <unistd.h>
%}

typedef unsigned int uid_t;


/*geuid的异常处理*/
%exception getuid{
  $action
  if(!result){
    jclass clazz = jenv -> FindClass("java/lang/OutOfMemoryError");
    jenv -> ThrowNew(clazz, "Out Of Memory");
    return $null;
  }
}
extern uid_t getuid(void);


%{
class AsyncUidProvider{
public:
   AsyncUidProvider(){}
   
   virtual ~AsyncUidProvider(){}
   
   void get(){
      onUid(getuid());
   }
   
   virtual void onUid(uid_t uid){}
};
%}

class AsyncUidProvider{
public:
   AsyncUidProvider();
   virtual ~AsyncUidProvider();
   
   void get();
   
   virtual void onUid(uid_t uid);
};

/*启用类的directors*/
%feature("director") AsyncUidProvider;


