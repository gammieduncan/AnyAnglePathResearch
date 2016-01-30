#include <cheerp/client.h>
#include <cheerp/clientlib.h>

using namespace client;

void webMain()
{
    document.addEventListener("DOMContentLoaded",cheerp::Callback(loadCallback));
}