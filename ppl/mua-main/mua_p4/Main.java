package mua;
import java.util.List;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;
import java.util.Scanner;
import java.util.Stack;
import java.util.Map.Entry;
import java.io.*;


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
            if(s.equals("readlist")) //readlist command, read one line
            {
                readin.add(s);
                in.nextLine();
                s = in.nextLine();
                String [] s2 = s.split(" ");
                for(int i = 0; i < s2.length; i++)
                    if(!s2[i].isEmpty()) readin.add(s2[i]);
                readin.add("!readlistend");
            }
            else
            {
                for(int i = 0; i < s1.length(); i++)
            {
                if(s1.charAt(i) == '[') s1.insert(i+1, " ");
                if(s1.charAt(i) == ']') {s1.insert(i, " "); i++;}
            }
            String [] s2 = s1.toString().split(" ");
            for(int i = 0; i < s2.length; i++)
               if(!s2[i].isEmpty()) readin.add(s2[i]);
            }            
            //i++;            
        }
        new operation("Main", readin, 0, new HashMap<String, value>(), new HashMap<String, value>());
        in.close();
    }
}

class value
{
    String type = null;
    Object content = null;
    void PrintValue()
    {
        System.out.print(content);
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
    value NakedList()
    {
        List<Object> l = (List<Object>)content;
        List<Object> nl = l.subList(1, l.size()-1);
        return new list(nl);
    }
    @Override
    void PrintValue()
    {
        List<Object> l = (List<Object>)content;
        //System.out.print(l.get(0));
        for(int i = 0; i < l.size(); i++)
        {
            if(l.get(i) instanceof list)
            {
                System.out.print("[");
                ((list)(l.get(i))).NakedList().PrintValue();
                System.out.print("]");
            }
            else
            {
                if(l.get(i) instanceof value)
                {
                    ((value)(l.get(i))).PrintValue();
                    
                }
                else
                {
                    System.out.print(l.get(i));
                }
            }
            if(i != l.size()-1) System.out.print(" ");
        }        
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
    public Map<String, value> ClosureEnv = null;    //current operation's running closure environment
    static public Map<String, Map<String, value>> FunClosures = new HashMap<String, Map<String, value>>();  //store the closure environments for all the functions
    static public Stack<Map<String, value>> ClosureStack = new Stack<Map<String, value>>();
    operation(String fname, List<String> readin, int pid, Map<String, value> namelist, Map<String, value> env)
    {   
        name = fname;
        exec.addAll(readin);
        ClosureEnv = env;
        id = pid;
        //if(id == 0) LocalNameList = GlobalNameList; //if this is main fuction, then it's local variable list is the global list
        LocalNameList = namelist; //else create its own local list
        LocalNameList.put("pi", new number(3.14159));
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
        else if(exec.get(index).equals("readlist")) return op_readlist();
        else if(exec.get(index).equals("word")) return op_word();
        else if(exec.get(index).equals("list")) return op_list();
        else if(exec.get(index).equals("sentence")) return op_sentence();
        else if(exec.get(index).equals("join")) return op_join();
        else if(exec.get(index).equals("first")) return op_first();
        else if(exec.get(index).equals("last")) return op_last();
        else if(exec.get(index).equals("butfirst")) return op_butfirst();
        else if(exec.get(index).equals("butlast")) return op_butlast();
        else if(exec.get(index).equals("load")) return op_load();
        else if(exec.get(index).equals("save")) return op_save();
        else if(exec.get(index).equals("erall")) return op_erall();
        else 
        {
            operation fun_res = func();
            if(fun_res == null) return null;
            else
            {
                Map<String, value> this_fun_state = new HashMap<String, value>(fun_res.LocalNameList);
                this_fun_state.putAll(fun_res.ClosureEnv);
                ClosureStack.push(this_fun_state);
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
        else if(exec.get(index).equals("true")) return new bool(true);
        else if(exec.get(index).equals("false")) return new bool(false);
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
    }//private int f(int n){int res=1; for(int i=1; i<=n; i++) res*=i; return res;}
    value op_make()
    {
        index++;
        value dest_name = valuecheck();
        index++;
        value v = valuecheck();
        if(v != null && !ClosureStack.isEmpty() && v.type.equals("list") && ((List<Object>)v.content).get(1) instanceof list && ((List<Object>)v.content).get(2) instanceof list)
            FunClosures.put((String)dest_name.content, ClosureStack.pop());
            
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
            else if(ClosureEnv.get(exec.get(index).substring(1)) != null) return ClosureEnv.get(exec.get(index).substring(1));
            else return GlobalNameList.get(v.content.toString());
        } 
        else{
            if(LocalNameList.containsKey(exec.get(index))) return LocalNameList.get(exec.get(index));
            else if(ClosureEnv.get(exec.get(index).substring(1)) != null) return ClosureEnv.get(exec.get(index).substring(1));
            else return GlobalNameList.get(exec.get(index));
        }               
    }
    value op_colon() //:
    {
        if(LocalNameList.get(exec.get(index).substring(1)) != null) return LocalNameList.get(exec.get(index).substring(1));
        else if(ClosureEnv.get(exec.get(index).substring(1)) != null) return ClosureEnv.get(exec.get(index).substring(1));
        else return GlobalNameList.get(exec.get(index).substring(1));
    }
    
    value op_print()
    {
        index++;
        value res = valuecheck();
        if(res instanceof list) ((list)res).NakedList().PrintValue();
        else res.PrintValue();
        //ClosureStack.pop();
        System.out.println("");
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
            return new number(-1);            
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
        operation new_op = new operation(name, run_list.MyToString().subList(1, run_list.MyToString().size()-1 ), id+1 , LocalNameList, ClosureEnv);
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
        try 
        {
            double num = Double.valueOf(v1.content.toString());
            v1.type = "number";
            v1.content = num;
            num = Double.valueOf(v2.content.toString());
            v2.type = "number";
            v2.content = num;
        }
        catch(Exception e) {}
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
        if(fun == null && ClosureEnv != null) fun = (list)ClosureEnv.get(fun_name);  
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
            Map<String, value> new_closure_env = new HashMap<String, value>();
            
            
            new_closure_env.putAll(LocalNameList);
            if(FunClosures.get(fun_name) != null) new_closure_env.putAll(FunClosures.get(fun_name));
            new_closure_env.putAll(new_fun_namelist);
            new_closure_env.putAll(ClosureEnv);
            
            operation new_op = new operation(fun_name, exec_code, id + 1, new_fun_namelist, new_closure_env);
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
    //p4
    value op_readlist()
    {
        index++;
        List<Object> newlist_content = new ArrayList<Object>();
        newlist_content.add("[");
        while(!exec.get(index).equals("!readlistend")) {newlist_content.add(exec.get(index)); index++;}
        newlist_content.add("]");
        value newlist = new list(newlist_content);
        return newlist;
    }
    value op_word()
    {
        index++;
        value v1 = valuecheck();
        index++;
        value v2 = valuecheck();
        return new word(v1.content.toString()+v2.content);
    }
    value op_list()
    {
        index++;
        value v1 = valuecheck();
        index++;
        value v2 = valuecheck();
        List<Object> newlist_content = new ArrayList<Object>();
        newlist_content.add("[");
        if(v1 instanceof list) newlist_content.add(((list)v1));
        else newlist_content.add(v1);
        if(v2 instanceof list) newlist_content.add(((list)v2));
        else newlist_content.add(v2);
        newlist_content.add("]");
        value newlist = new list(newlist_content);
        return newlist;
    }
    value op_sentence()
    {
        index++;
        value v1 = valuecheck();
        index++;
        value v2 = valuecheck();
        List<Object> newlist_content = new ArrayList<Object>();
        newlist_content.add("[");
        if(v1 instanceof list)
        {
            List<Object> l1 = (List<Object>)(((list)v1).content);
            for(int i = 1; i < l1.size()-1; i++)
            {
                newlist_content.add(l1.get(i));
            }
        }
        else newlist_content.add(v1);
        if(v2 instanceof list)
        {
            List<Object> l2 = (List<Object>)(((list)v2).content);
            for(int i = 1; i < l2.size()-1; i++)
            {
                newlist_content.add(l2.get(i));
            }
        }
        else newlist_content.add(v2.content);
        newlist_content.add("]");
        value newlist = new list(newlist_content);
        return newlist;
    }
    value op_join()
    {
        index++;
        list v1 = (list)valuecheck();
        index++;
        value v2 = valuecheck();
        List<Object> l = (List<Object>)v1.content;
        l.add(l.size()-1, v2);
        return new list(l);
    }
    value op_first()
    {
        index++;
        value v = valuecheck();
        if(v instanceof list)
        {
            if(((List<Object>)v.content).get(1) instanceof value)
            return (value)(((List<Object>)v.content).get(1));
            else return new word(((List<Object>)v.content).get(1).toString());
        }
        else
        {
            return new word(v.content.toString().substring(0, 1));
        }
    }

    value op_last()
    {
        index++;
        value v = valuecheck();
        if(v instanceof list)
        {
            List<Object> l = (List<Object>)v.content;
            if(l.get(l.size()-2) instanceof value)
            {
                return (value)((l.get(l.size()-2)));
            }
            else return new word(l.get(l.size()-2).toString());
        }
        else
        {
            String s = v.content.toString();
            if(s.endsWith(".0")) return new word(s.substring(s.length()-3, s.length()-2));
            return new word(s.substring(s.length()-1, s.length()));
        }
    }

    value op_butfirst()
    {
        index++;
        value v = valuecheck();
        if(v instanceof list)
        {
            List<Object> l = (List<Object>)v.content;
            List<Object> ll = l.subList(2, l.size()-1);
            ll.add(0, "["); ll.add("]");
            return new list(ll);
        }
        else
        {
            String s = v.content.toString();
            return new word(s.substring(1, s.length()));
        }
    }
    value op_butlast()
    {
        index++;
        value v = valuecheck();
        if(v instanceof list)
        {
            List<Object> l = (List<Object>)v.content;
            List<Object> ll = l.subList(1, l.size()-2);
            ll.add(0, "["); ll.add("]");
            return new list(ll);
        }
        else
        {
            String s = v.content.toString();
            return new word(s.substring(0, s.length()-1));
        }
    }
    value op_random()
    {
        number n = numbercheck();
        return new number(Math.random()*((double)(n.content)+1));
    }
    value op_int()
    {
        number n = numbercheck();
        return new number((int)(n.content));
    }
    value op_sqrt()
    {
        number n = numbercheck();
        return new number(Math.sqrt(((double)(n.content))));
    }
    value op_erall()
    {
        LocalNameList.clear();
        if(id == 0) GlobalNameList.clear();
        return new bool(true);
    }
    value op_save()
    {
        index++;
        value v;
        v = valuecheck();
        try
        {
            
            File file = new File(v.content.toString());
            FileOutputStream fileout = new FileOutputStream(file);
            for(Map.Entry<String, value> name : LocalNameList.entrySet())
            {
                fileout.write(("make \"" + name.getKey() + " \""+ name.getValue().content + " ").getBytes());
            }
            fileout.close();
        }
        catch (Exception e)
        {
            System.out.println("save error");            
        }
        return v;
    }
    value op_load()
    {
        index++;
        value v;
        v = valuecheck();
        try
        {
            File file = new File(v.content.toString());
            FileInputStream filein = new FileInputStream(file);
            byte [] b = new byte[(int)(file.length())];
            filein.read(b);
            String s = new String(b);
            String [] ss = s.split(" ");
            for(int i = 0; i < ss.length; i++)
            {
                exec.add(index + 1 + i, ss[i]);
            }
            filein.close();
        }
        catch(Exception e)
        {
            System.out.println("load error");
        }
        return new bool(true);
    }
}
