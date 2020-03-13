#include "environment.h"
#include <iostream>
using namespace std;
using namespace LANG_NS;

int RunFromString(Environment& env, const BytesT& str, const ValuePtrList& paramss)
{
    (void)env.Call(env.LoadString(Unicode::Decode(str)), paramss);
    return 0;
}

int RunFromFile(Environment& env, const BytesT& file_name, const ValuePtrList& paramss)
{
    (void)env.Call(env.LoadFile(Unicode::Decode(file_name)), paramss);
    return 0;
}

int RunCommand(Environment& env)
{
    BytesT buffer;
    do
    {
        BytesT cmd;
        if (!buffer.empty())
        {
            cout << ">";
        }
        cout << "> ";
        getline(cin, cmd);
        if (cmd == "quit")
        {
            break;
        }

        if (cmd.empty())
        {
            continue;
        }

        bool run = true;
        if (cmd[cmd.size() - 1] == ':')
        {
            cmd[cmd.size() - 1] = '\n';
            run = false;
        }
        buffer += cmd;
        if (run)
        {
            RunFromString(env, buffer, {});
            buffer.clear();
        }
    } while (true);
    return 0;
}

int main(int argc, char** argv)
{
    Environment& env = Environment::GetInstance();
    if (argc == 1)
    {
        return RunCommand(env);
    }
    else if (argc >= 2)
    {
        if (strcmp(argv[1], "-e") == 0)
        {
            if (argc == 2)
            {
                cout << "need a string after -e!!!";
                return 0;
            }
            BytesT str = argv[2];
            ValuePtrList params;
            for (int i = 3; i < argc; ++i)
            {
                params.push_back(Value::New(argv[i]));
            }
            return RunFromString(env, argv[2], params);
        }
        else
        {
            ValuePtrList params;
            for (int i = 2; i < argc; ++i)
            {
                params.push_back(Value::New(argv[i]));
            }
            return RunFromFile(env, argv[1], params);
        }
    }
    return 0;
}
