//package mua;
import java.util.List;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;
import java.util.Scanner;
import java.util.Stack;
import java.lang.Exception;


public class Main {    
    public static void main(String[] args) {
        Scanner in = new Scanner(System.in); 
        List<String> readin = new ArrayList<String>();
        //int i = 0;
        while(in.hasNext())
        {
            String s = in.next();
            StringBuffer s1 = new StringBuffer(s);
            if(s.equals("end")) break;
            //String s1 = s;
            for(int i = 0; i < s1.length(); i++)
            {
                if(s1.charAt(i) == '[') s1.insert(i+1, " ");
                if(s1.charAt(i) == ']') {s1.insert(i, " "); i++;}
            }
            String [] s2 = s1.toString().split(" ");
            for(int i = 0; i < s2.length; i++)
               if(!s2[i].isEmpty()) readin.add(s2[i]);
            //i++;            
        }
        new operation("Main", readin, 0, new HashMap<String, value>());
        in.close();
    }
}

class value
{
    String type = null;
    Object content = null;
    void PrintValue()
    {
        System.out.println(content);
        //if(type.equals("list")) System.out.println(((List<Object>)content).size());
    }
}
class word extends value
{
    word()
    {
        type = "word";
        content = "";
    }
    word(String s)
    {
        type = "word";
        content = s;
    }
    

}
class number extends value
{

    number(double x)
    {
        type = "number";
        content = x;
    }

}
class bool extends value
{
    bool(boolean x)
    {
        type = "bool";
        content = x;
    }
}
class list extends value
{
    list(List<Object> x)
    {
        type = "list";
        content = x;
    }
    @Override
    void PrintValue()
    {
        List<Object> l = (List<Object>)content;
        for(int i = 0; i < l.size(); i++)
        {
            if(l.get(i) instanceof list)
            {
                ((list)(l.get(i))).PrintValue();
            }
            else
            {
                System.out.print(l.get(i) + " ");
            }
        }
        System.out.println("");
    }
    List<String> MyToString()//change the 'class list' elements in the content
    {
        List<String> res = new ArrayList<String>();
        for(Object i: (List<Object>)content)
        {
            if(i instanceof String) res.add(i.toString());
            else if(i instanceof list) 
            {
                res.addAll(((list)i).MyToString());
            }
        }
        return res;
    }
}
class operation
{
    List<String> exec = new ArrayList<String>();
    int id; //if this is main function
    int index = 0;
    value final_v;
    String name;
    static public Map<String, value> GlobalNameList = new HashMap<String, value>();
    public Map<String, value> LocalNameList = null;
    //public Map<String, value> ClosureEnv = null;    //current operation's running closure environment
    //static public Map<String, Map<String, value>> FunClosures = new HashMap<String, Map<String, value>>();  //store the closure environments for all the functions
    //static public Stack<Map<String, value>> ClosureStack = new Stack<Map<String, value>>();
    operation(String fname, List<String> readin, int pid, Map<String, value> namelist)
    {   
        name = fname;
        exec.addAll(readin);
        //ClosureEnv = env;
        id = pid;
        //if(id == 0) LocalNameList = GlobalNameList; //if this is main fuction, then it's local variable list is the global list
        LocalNameList = namelist; //else create its own local list
        while(index < exec.size())
            final_v = exec_operation();
    }
    value exec_operation()
    {
        value res = opcheck();
        //opcheck();
        index++;
        return res;
    }
    //index: the execution position now

    //check amd execute the following instruction
    value opcheck()
    {
        if(exec.get(index).equals("make")) return op_make();
        else if(exec.get(index).equals("thing")) return op_thing();
        else if(exec.get(index).startsWith(":")) return op_colon();
        else if(exec.get(index).equals("print")) return op_print();
        else if(exec.get(index).equals("read")) return op_read();
        else if(exec.get(index).equals("add")|| exec.get(index).equals("sub") || exec.get(index).equals("mul")|| exec.get(index).equals("div") || exec.get(index).equals("mod")) return op_operator(exec.get(index));
        else if(exec.get(index).equals("run")) return op_run();
        else if(exec.get(index).equals("gt") || exec.get(index).equals("lt") || exec.get(index).equals("eq")) return op_cmp(exec.get(index));
        else if(exec.get(index).equals("not") || exec.get(index).equals("and") || exec.get(index).equals("or")) return op_logic(exec.get(index));
        else if(exec.get(index).equals("isnumber") || exec.get(index).equals("isword") || exec.get(index).equals("islist") || exec.get(index).equals("isbool") || exec.get(index).equals("isempty")) return op_isvalue(exec.get(index));
        else if(exec.get(index).equals("isname")) return op_isname();
        else if(exec.get(index).equals("erase")) return op_erase();
        else if(exec.get(index).equals("if")) return op_if();
        else if(exec.get(index).equals("return")) return op_return();
        else if(exec.get(index).equals("export")) return op_export();
        else 
        {
            operation fun_res = func();
            if(fun_res == null) return null;
            else
            {
                Map<String, value> this_fun_state = new HashMap<String, value>(fun_res.LocalNameList);
                // this_fun_state.putAll(fun_res.ClosureEnv);
                // ClosureStack.push(this_fun_state);
                //FunClosures.put(fun_res.name, this_fun_state);
                return fun_res.final_v;
            }
        }
    }

    //check the next value(instruction -> execute and get the result; name -> get from the list)
    value valuecheck()
    {
        if(exec.get(index).startsWith("[")) return read_list();
        else if(exec.get(index).startsWith("\"")) return getimm(exec.get(index).substring(1)); //"xxxxxx
        else if(exec.get(index).startsWith(":")) return op_colon(); //word
        else
        {
            value res = opcheck();
            if(res != null) return res;
            else return numbercheck(); //inst
        }
    }
    list read_list()
    {
        List<Object> new_list_content = new ArrayList<Object>();
        if(!exec.get(index).equals("[")) return null; //not a list format
        new_list_content.add("[");
        index++;
        while(!exec.get(index).equals("]"))
        {
            if(exec.get(index).equals("[")) new_list_content.add(read_list());//recursive list
            else if(!exec.get(index).isEmpty()) new_list_content.add(exec.get(index));
            index++;
        }
        //if(!exec.get(index).equals("]")) new_list_content.add(exec.get(index).substring(0, exec.get(index).length()-1));
        new_list_content.add("]");
        list new_list = new list(new_list_content);
        return new_list;
    }
    value getimm(String x)
    {
        try
        {
            double num = Double.valueOf(x);
            return new number(num);
        }
        catch(Exception e)
        {
            return new word(x);
        }
    }
    number numbercheck()
    {
        try
        {
            double num;
            if(exec.get(index).startsWith("\"")) num = Double.valueOf(exec.get(index).substring(1));
            else num = Double.valueOf(exec.get(index)); //if the word is a number itself
            return new number(num);
        }
        catch(Exception e)
        {
            value v = opcheck();
            if(v != null && v.type.equals("number"))
            {
                return new number((Double)v.content);
            }
            return null;
        }
    }private int f(int n){int res=1; for(int i=1; i<=n; i++) res*=i; return res;}
    value op_make()
    {
        index++;
        value dest_name = valuecheck();
        index++;
        value v = valuecheck();
        // if(v != null && !ClosureStack.isEmpty() && v.type.equals("list") && ((List<Object>)v.content).get(1) instanceof list && ((List<Object>)v.content).get(2) instanceof list)
        //     FunClosures.put((String)dest_name.content, ClosureStack.pop());
            
        LocalNameList.put(dest_name.content.toString(), v);
        if(id == 0) GlobalNameList.put(dest_name.content.toString(), v);
        return v;
        
    }
    value op_thing() //thing
    {
        index++;
        //if(!exec.get(index).startsWith("\"")) System.out.println("Invalid Format");
        value v = valuecheck();
        if(v != null){
            if(LocalNameList.containsKey(v.content.toString())) return LocalNameList.get(v.content.toString());
            //else if(ClosureEnv.get(exec.get(index).substring(1)) != null) return ClosureEnv.get(exec.get(index).substring(1));
            else return GlobalNameList.get(v.content.toString());
        } 
        else{
            if(LocalNameList.containsKey(exec.get(index))) return LocalNameList.get(exec.get(index));
            //else if(ClosureEnv.get(exec.get(index).substring(1)) != null) return ClosureEnv.get(exec.get(index).substring(1));
            else return GlobalNameList.get(exec.get(index));
        }               
    }
    value op_colon() //:
    {
        if(LocalNameList.get(exec.get(index).substring(1)) != null) return LocalNameList.get(exec.get(index).substring(1));
        //else if(ClosureEnv.get(exec.get(index).substring(1)) != null) return ClosureEnv.get(exec.get(index).substring(1));
        else return GlobalNameList.get(exec.get(index).substring(1));
    }
    
    value op_print()
    {
        index++;
        value res = valuecheck();
        res.PrintValue();
        //ClosureStack.pop();
        return res;
    }
    value op_read()
    {
        value readin;
        index++;
        String s = exec.get(index);
        try 
        {
            double num = Double.valueOf(s);
            readin = new number(num);
        }
        catch(Exception e)
        {
            readin = new word(s);
        }
        //in1.close();
        return readin;
    }
    
    number op_operator(String type)
    {
        index++;
        number v1 = numbercheck();
        index++;
        number v2 = numbercheck();
        try {
            if(type.equals("add")) return new number((double)v1.content + (double)v2.content);
            else if(type.equals("sub")) return new number((double)v1.content - (double)v2.content);
            else if(type.equals("mul")) return new number((double)v1.content * (double)v2.content);
            else if(type.equals("div")) return new number((double)v1.content / (double)v2.content);
            else if(type.equals("mod")) return new number((double)v1.content % (double)v2.content);
            else return null;
        }
        catch(Exception e)
        {
            return new number(f(((Double)v1.content).intValue()));            
        }        
    }
    
    //p2
    value op_erase()
    {
        index++;
        value res;
        res = LocalNameList.get(exec.get(index).substring(1));
        LocalNameList.remove(exec.get(index).substring(1));
        return res;        
    }
    bool op_isname()
    {
        index++;
        if(LocalNameList.containsKey(exec.get(index).substring(1)))
        return new bool(true);
        else return new bool(false);
    }
    value op_run()
    {
        index++;        
        list run_list = LocalNameList.containsKey(exec.get(index).substring(1))? (list)LocalNameList.get(exec.get(index).substring(1)): (list)GlobalNameList.get(exec.get(index).substring(1));
        if(run_list == null) run_list = read_list(); 
        if(run_list == null || ((List<Object>)(run_list.content)).size() ==2) return run_list;//not a list || empty list
        if(((List<Object>)(run_list.content)).size() == 3) return getimm((String)((List<Object>)(run_list.content)).get(1)); //only an non-op element in the list(for if operation)  
        operation new_op = new operation(name, run_list.MyToString().subList(1, run_list.MyToString().size()-1 ), id+1 , LocalNameList);
        value res = new_op.final_v;
        LocalNameList.putAll(new_op.LocalNameList); //the run list will operate the variables
        return res;
    }
    bool op_cmp(String type)
    {
        index++;
        value v1 = valuecheck();         
        index++;
        value v2 = valuecheck();
        boolean number_cmp = v1.type.equals("number")&v2.type.equals("number");
        if(type.equals("eq")) return number_cmp? new bool(((Double)v1.content).equals((Double)v2.content)): new bool(v1.content.toString().compareTo(v2.content.toString()) == 0);
        else if(type.equals("gt")) return number_cmp? new bool((Double)v1.content > (Double)v2.content): new bool(v1.content.toString().compareTo(v2.content.toString()) > 0);
        else if(type.equals("lt")) return number_cmp? new bool((Double)v1.content < (Double)v2.content): new bool(v1.content.toString().compareTo(v2.content.toString()) < 0);
        else return null;
    }
    bool op_logic(String type)
    {
        if(type.equals("not"))
        {
            index++;
            bool v = (bool)valuecheck();
            return new bool(!(Boolean)v.content);
        }
        else
        {
            index++;
            bool v1 = (bool)valuecheck();
            index++;
            bool v2 = (bool)valuecheck();
            if(type.equals("and")) return new bool((boolean)v1.content & (boolean)v2.content);
            else if(type.equals("or")) return new bool((boolean)v1.content | (boolean)v2.content);
            else return null;
        }
    }
    bool op_isvalue(String type)
    {
        index++;
        value v = valuecheck();
        //v.PrintValue();
        if(type.equals("isnumber")) return new bool(v.type.equals("number"));
        else if(type.equals("isword")) return new bool(v.type.equals("word"));
        else if(type.equals("islist")) return new bool(v.type.equals("list"));
        else if(type.equals("isbool")) return new bool(v.type.equals("bool"));
        else if(type.equals("isempty")) return new bool( (v.type.equals("word") && ((String)v.content).isEmpty()) || (v.type.equals("list") && ((List<Object>)v.content).size()==2));
        else return null;
    }
    value op_if()
    {
        index++;
        bool statement = (bool)valuecheck();
        value res;
        if((Boolean)statement.content) //run the first list
        {
            //index++;
            //System.out.println("1");
            res = op_run();
            index++;
            read_list();
        }
        else
        {
            index++;
            read_list();
            res = op_run();
    
        }
        return res;
    }
    operation func()
    {
        String fun_name = exec.get(index);
        list fun = (list)LocalNameList.get(fun_name); 
        //if(fun == null && ClosureEnv != null) fun = (list)ClosureEnv.get(fun_name);  
        if( fun != null && fun.type.equals("list") && ((List<Object>)fun.content).get(1) instanceof list && ((List<Object>)fun.content).get(2) instanceof list)//is a function name
        {
            //System.out.println("hahahahaa");
            Map<String, value> new_fun_namelist = new HashMap<String, value>();  
            new_fun_namelist.put(exec.get(index), fun);  
            list arg = (list)((List<Object>)fun.content).get(1);
            List<String> arg_content = ((List<String>)arg.content);
            list code = (list)((List<Object>)fun.content).get(2);
            List<String> exec_code = code.MyToString().subList(1, code.MyToString().size()-1);
            for(int i = 1; i < arg_content.size() - 1; i++)
            {
                index++;
                value v = valuecheck();
                new_fun_namelist.put(arg_content.get(i), v); //add values to new function's namelist
            }
            new_fun_namelist.putAll(LocalNameList);
            
            operation new_op = new operation(fun_name, exec_code, id + 1, new_fun_namelist);
            return new_op;
        }
        else
        {
            return null;
        }
    }
    value op_export()
    {
        index++;
        value dest_name = valuecheck();
        value v = LocalNameList.get(dest_name.content.toString());
        GlobalNameList.put(dest_name.content.toString(), v);
        return v;
    }
    value op_return()
    {
        index++;
        value v = valuecheck();
        //ClosureStack.pop();
        index = exec.size();
        return v;
    }
}
