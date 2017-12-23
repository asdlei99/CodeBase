#include <iostream>
#include "person.pb.h"

using namespace std;

int main()
{
    char buf[1024];
    int len;

    GOOGLE_PROTOBUF_VERIFY_VERSION;

    Person obj;
    obj.set_name("gongluck");
    obj.set_id(1);
    *obj.mutable_email() = "http://blog.csdn.net/gongluck93";
    len = obj.ByteSize();
    cout << "len = " << len << endl;
    obj.SerializeToArray(buf, len);

    Person obj2;
    obj2.ParseFromArray(buf, len);
    cout << "name = " << obj2.name() << endl;
    cout << "id = " << obj2.id() << endl;
    cout << "email = " << obj2.email() << endl;

    google::protobuf::ShutdownProtobufLibrary();

    return 0;
}
