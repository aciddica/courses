import java.util.Map;
import java.util.List;
import java.util.ArrayList;
import java.util.Comparator;
import java.util.HashMap;
import java.util.Iterator;
import java.util.TreeSet;
import java.util.Set;
import java.util.Scanner;
public class score
{
    public static void main(String[] args) {
        Scanner in = new Scanner(System.in);
        String cin = in.nextLine();
        List<Student> students = new ArrayList<Student>();
        while(!cin.equals("END"))
        {
            
            String [] words = new String [5];
            words = cin.split(", ");
            
            if(words.length == 2)
            {
                int flag = 0;
                for(int i = 0; i < students.size(); i++)
                {
                    if(words[0].equals(students.get(i).id))
                    {
                        flag = 1;
                        students.get(i).addName(words[1]);
                        break;
                    }
                    
                }
                if(flag == 0)
                {
                    Student newstu = new Student(words[0], words[1]);
                    students.add(newstu);
                }
                
            }
            else if(words.length == 3)
            {
                int flag = 0;
                for(int i = 0; i < students.size(); i++)
                {
                    if(words[0].equals(students.get(i).id))
                    {
                        flag = 1;
                        students.get(i).addCourse(words[1], Double.valueOf(words[2]));
                        break;
                    }
                    
                }
                if(flag == 0)
                {
                    Student newstu = new Student(words[0]);
                    newstu.addCourse(words[1], Double.valueOf(words[2]));
                    students.add(newstu);
                }
            }
            else System.out.println("error in length");
            cin = in.nextLine();
        }
        students.sort(new Comparator<Student>(){
            @Override
            public int compare(Student s1, Student s2)
            {
                return s1.id.compareTo(s2.id);
            }
        });
        for(int i = 0; i < students.size(); i++)
        {
            if(i == 0) students.get(i).PrintStu(true);
            else students.get(i).PrintStu(false);
        }
        

        in.close();
    }
    
}
class Student
{
    String id; 
    String name;
    Map<String, Double> clist = new HashMap<String, Double>();
    Student(String nid, String nname)
    {
        id = nid;
        name = nname;
    }
    Student(String sid)
    {
        id = sid;
        name = "noname";
    }
    static Set<String> courses = new TreeSet<String>();
    static Set<String> sortcourses = new TreeSet<String>(new Comparator<String>(){
        @Override
        public int compare(String s1, String s2)
        {
            return s1.compareTo(s2);
        }
    });
    void PrintHead()
    {
        sortcourses.addAll(courses);
        System.out.print("student id, name, ");
        for(String x : sortcourses)
            System.out.print(x + ", ");
        System.out.println("average");
         
    }
    void addName(String sname)
    {
        name = sname;
    }
    void addCourse(String cname, Double score)
    {
        clist.put(cname, score);
        courses.add(cname);
    }
    void PrintStu(boolean head)
    {
        if(head) PrintHead();

        System.out.print(id + ", " + name + ", ");
        double sumscore = 0;
        int num = 0;
        Iterator<String> it = sortcourses.iterator();
        while(it.hasNext())
        {
            String cname = it.next();
            if(clist.containsKey(cname))
            {
                System.out.printf("%.1f", clist.get(cname));
                System.out.print(", ");
                sumscore += clist.get(cname);
                num++;
            }
            else
            {
                System.out.print(" , ");
            }
        }
        System.out.printf("%.1f", sumscore / num);
        System.out.println("");
        
    }

    
}
