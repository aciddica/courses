
import javax.swing.*;
import java.awt.event.*;
import java.util.ArrayList;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.BasicStroke;
import java.awt.Color;
import java.awt.FlowLayout;
import java.awt.BorderLayout;
import java.awt.Font;
import java.io.*;


public class miniCAD extends JFrame {
    
    DrawPanel p = null;
    JPanel buttons = null;
    public miniCAD()
    {
        buttons_init();
        this.setTitle("mini CAD");        
        this.setVisible(true);
        this.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        this.setSize(1200, 800);
        this.setLocation(200, 100);
        this.setLayout(new BorderLayout());
 

        this.add(buttons, BorderLayout.NORTH);
        p = new DrawPanel();
        this.addMouseListener(p);
        this.addMouseMotionListener(p);
        this.addKeyListener(p);
        this.add(p);
    }
    
    public static void main(String[] args) {
        new miniCAD();
    }
    
    void buttons_init()//initialize the buttons
    {
        buttons = new JPanel();//control button
        JButton line = new JButton("Line"); //draw line
        line.addActionListener(new ActionListener(){
            public void actionPerformed(ActionEvent e)
            {                
                p.draw_choice = 1;
                p.click_ctrl = 1;
                p.requestFocus();//return the focus to the background
            }
        });
        buttons.add(line);
        buttons.setLayout(new FlowLayout());
        JButton rect= new JButton("Rectangular");//draw rectangular
        rect.addActionListener(new ActionListener(){
            public void actionPerformed(ActionEvent e)
            {
                p.draw_choice = 2;
                p.click_ctrl = 1;
                p.requestFocus();
            }
        });
        buttons.add(rect);
        JButton circle = new JButton("Circle");//draw circle 
        circle.addActionListener(new ActionListener(){
            public void actionPerformed(ActionEvent e)
            {
                p.draw_choice = 3;
                p.click_ctrl = 1;
                p.requestFocus();
            }
        });
        buttons.add(circle);

        JButton text = new JButton("Text");//draw text
        text.addActionListener(new ActionListener(){
            public void actionPerformed(ActionEvent e)
            {
                JFrame new_window = new JFrame();//jump the input window
                new_window.setVisible(true);
                new_window.setLocation(800, 500);
                new_window.setTitle("Input a string");
                new_window.setSize(300, 100);
                new_window.setLayout(new FlowLayout());
                JTextField input = new JTextField(20);
                JButton OK = new JButton("OK");
                OK.addActionListener(new ActionListener(){
                    public void actionPerformed(ActionEvent e)
                    {
                        p.input_text = input.getText();
                        new_window.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
                        new_window.dispose();
                        p.requestFocus();
                    }
                });
                new_window.add(input);
                new_window.add(OK);
                p.draw_choice = 4;
                p.click_ctrl = 1;
                
            }
        });
        buttons.add(text);
        JButton del = new JButton("delete");//delete 
        del.addActionListener(new ActionListener(){
            public void actionPerformed(ActionEvent e)
            {
                p.draw_choice = 5;
                p.click_ctrl = 1;
                p.requestFocus();
                if(p.pattern_choice != -1) {p.patterns.remove(p.pattern_choice); p.repaint();}//delete selected
            }
        });
        buttons.add(del);

        JButton save = new JButton("save");//save
        save.addActionListener(new ActionListener(){
            public void actionPerformed(ActionEvent e)
            {
                JFileChooser chooser = new JFileChooser();
                int ret_val = chooser.showSaveDialog(null);
                chooser.setDialogTitle("save file");
                chooser.setFileSelectionMode(JFileChooser.DIRECTORIES_ONLY);
                if(ret_val == JFileChooser.APPROVE_OPTION)
                {
                    try{
                        FileOutputStream f = new FileOutputStream(new File(chooser.getSelectedFile().getAbsolutePath()));
                        System.out.println(chooser.getSelectedFile());
                        ObjectOutputStream objf = new ObjectOutputStream(f);
                        objf.writeObject(p.patterns);                        
                    }
                    catch(Exception exp)
                    {
                        System.out.println("Save error :" + exp.getMessage());
                    }
                    p.requestFocus();
                }               
            }
        });
        buttons.add(save);
        JButton load = new JButton("load");//load
        load.addActionListener(new ActionListener(){
            @SuppressWarnings("unchecked")
            public void actionPerformed(ActionEvent e)
            {
                JFileChooser chooser = new JFileChooser();
                int ret_val = chooser.showOpenDialog(null);
                chooser.setDialogTitle("load file");
                chooser.setFileSelectionMode(JFileChooser.FILES_ONLY);
                if(ret_val == JFileChooser.APPROVE_OPTION)
                {
                    try{
                        FileInputStream f = new FileInputStream(new File(chooser.getSelectedFile().getAbsolutePath()));
                        System.out.println(chooser.getSelectedFile());
                        ObjectInputStream objf = new ObjectInputStream(f);
                        p.patterns = (ArrayList<DrawUnit>)objf.readObject();
                        System.out.println(p.patterns.get(0).x1 + "," + p.patterns.get(0).y1 + " " + p.patterns.get(0).color);
                        p.repaint();
                    }
                    catch(Exception exp)
                    {
                        System.out.println("Load Error:" + exp.getMessage());
                    }
                    p.requestFocus();
                }  
            }
        });
        buttons.add(load);
        JButton color_black = new JButton("");//set the color to black
        color_black.setBackground(Color.BLACK);
        color_black.addActionListener(new ActionListener(){
            public void actionPerformed(ActionEvent e)
            {
                p.set_color = 1;
                p.click_ctrl = 0;
                p.requestFocus();
                if(p.pattern_choice != -1) {p.patterns.get(p.pattern_choice).ColorChange(1); p.repaint();}//change the pattern's color 
            }
        });
        buttons.add(color_black);
        JButton color_red = new JButton("");//set the color to red
        color_red.setBackground(Color.RED);
        color_red.addActionListener(new ActionListener(){
            public void actionPerformed(ActionEvent e)
            {
                p.set_color = 2;
                p.click_ctrl = 0;
                p.requestFocus();
                if(p.pattern_choice != -1) {p.patterns.get(p.pattern_choice).ColorChange(2); p.repaint();}
            }
        });
        buttons.add(color_red);
        JButton color_green = new JButton("");//set the color to green
        color_green.setBackground(Color.GREEN);
        color_green.addActionListener(new ActionListener(){
            public void actionPerformed(ActionEvent e)
            {
                p.set_color = 3;
                p.click_ctrl = 0;
                p.requestFocus();
                if(p.pattern_choice != -1) {p.patterns.get(p.pattern_choice).ColorChange(3); p.repaint();}
            }
        });
        buttons.add(color_green);
        JButton color_yellow = new JButton("");//set the color to yellow
        color_yellow.setBackground(Color.YELLOW);
        color_yellow.addActionListener(new ActionListener(){
            public void actionPerformed(ActionEvent e)
            {
                p.set_color = 4;
                p.click_ctrl = 0;
                p.requestFocus();
                if(p.pattern_choice != -1) {p.patterns.get(p.pattern_choice).ColorChange(4); p.repaint();}
            }
        });
        buttons.add(color_yellow);
        JButton color_blue = new JButton("");//set the color to blue
        color_blue.setBackground(Color.BLUE);
        color_blue.addActionListener(new ActionListener(){
            public void actionPerformed(ActionEvent e)
            {
                p.set_color = 5;
                p.click_ctrl = 0;
                p.requestFocus();
                if(p.pattern_choice != -1) {p.patterns.get(p.pattern_choice).ColorChange(5); p.repaint();}
            }
        });
        buttons.add(color_blue);
        JButton color_white = new JButton("");//set the color to white
        color_white.setBackground(Color.WHITE);
        color_white.addActionListener(new ActionListener(){
            public void actionPerformed(ActionEvent e)
            {
                p.set_color = 0;
                p.click_ctrl = 0;
                p.requestFocus();
                if(p.pattern_choice != -1) {p.patterns.get(p.pattern_choice).ColorChange(0); p.repaint();}
            }
        });
        buttons.add(color_white);
    }
}
class DrawPanel extends JPanel implements MouseMotionListener, MouseListener, KeyListener
{
    ArrayList<DrawUnit> patterns = null;
    int draw_choice;//draw statues
    int pattern_choice;//the pattern we select
    int set_color;//color of brush
    int click_ctrl;//click time,control the drawing procedure
    int x1, y1, x2, y2;
    String input_text;
    public DrawPanel()
    {
        patterns = new ArrayList<DrawUnit>();
        this.addMouseListener(this);
        this.addMouseMotionListener(this);
        this.addKeyListener(this);
        draw_choice = 0;
        pattern_choice = -1;
        set_color = 1;
        click_ctrl = 0;       
    }

    public void paint(Graphics g)
    {
        super.paint(g);
        for(int i = 0; i < patterns.size(); i++)
        {
            //System.out.println("DrawPanel.paint()");
            
            if(i == pattern_choice) { patterns.get(i).DrawRange(g);}//draw the outline
            patterns.get(i).Set(g);            
            patterns.get(i).repaint(g);
        }
    }
    public void mouseDragged(MouseEvent e)//draw when dragging
    {   
        int newx = e.getX();
        int newy = e.getY();
        int cx = (patterns.get(pattern_choice).x1 + patterns.get(pattern_choice).x2) / 2;
        int cy = (patterns.get(pattern_choice).y1 + patterns.get(pattern_choice).y2) / 2;
        patterns.get(pattern_choice).Drag(newx - cx, newy-cy);
        this.repaint();
        //System.out.println("drag  draw_choice:" + draw_choice + " click_control:" + click_ctrl + " (" + newx + "," + newy + ")");
    }
    public void mouseMoved(MouseEvent e) {}
    public void mouseReleased(MouseEvent e) {}
    public void mouseEntered(MouseEvent e) {}
    public void mousePressed(MouseEvent e) {}
    public void mouseExited(MouseEvent e) {}
    public void mouseClicked(MouseEvent e)
    {
        System.out.print("click  draw_choice:" + draw_choice + " click_control:" + click_ctrl);
        if(click_ctrl == 1)//draw the first point
        {
            x1 = e.getX();
            y1 = e.getY();
            click_ctrl++;
        }
        else if(click_ctrl == 2)//draw the seconde point
        {
            x2 = e.getX();
            y2 = e.getY();
            if(draw_choice == 1)
            {
                DrawLine new_pattern = new DrawLine(x1, y1, x2, y2, set_color);
                patterns.add(new_pattern);
            }
            else if(draw_choice == 2)
            {
                DrawRect new_pattern = new DrawRect(x1, y1, x2, y2, set_color);
                patterns.add(new_pattern);
            }
            else if(draw_choice == 3)
            {
                DrawCircle new_pattern = new DrawCircle(x1, y1, x2, y2, set_color);
                patterns.add(new_pattern);
            }
            else if(draw_choice == 4)
            {
                DrawUnit new_pattern = new DrawText(x1, y1, x2, y2, set_color, input_text);
                patterns.add(new_pattern);
            }
            click_ctrl = 0;
        }
        else if(click_ctrl == 0)//select pattern
        {
            int xx = e.getX();
            int yy = e.getY();
            pattern_choice = -1;
            for(int i = patterns.size()-1; i >= 0; i--)
            {
                DrawUnit u = patterns.get(i);
                int left, top, right, bottom;//range 
                left = (u.x1<u.x2)?u.x1:u.x2;//left side
                top = (u.y1<u.y2)?u.y1:u.y2;//top side
                right = (u.x1>u.x2)?u.x1:u.x2;
                bottom = (u.y1>u.y2)?u.y1:u.y2;
                if( xx >= left && xx <= right && yy >= top && yy <= bottom)
                {
                    pattern_choice = i;
                    break;
                }
            }
            if(draw_choice == 5 && pattern_choice != -1) patterns.remove(pattern_choice);
            System.out.print("  index   " + pattern_choice);
        }
        this.repaint();
        System.out.println(" (" + x1 + "," + y1 + ") and (" + x2 + "," + y2 + ")" + " size:" + patterns.size());
    }
    public void keyTyped(KeyEvent e) {}
    public void keyReleased(KeyEvent e) {}
    public void keyPressed(KeyEvent e)
    {
        System.out.println("pressed " + e.getKeyCode());
        if(pattern_choice != -1)
        {
            if(e.getKeyCode() == 107) patterns.get(pattern_choice).ReShape(1);
            if(e.getKeyCode() == 109) patterns.get(pattern_choice).ReShape(-1);
            if(e.getKeyCode() == 46) patterns.get(pattern_choice).BrushChange(1);
            if(e.getKeyCode() == 44) patterns.get(pattern_choice).BrushChange(-1);
            this.repaint();
        }
    }
}
abstract class DrawUnit implements Serializable{
    int x1, y1, x2, y2, brush, color;
    public DrawUnit(int a, int b, int c, int d, int f)
    {
        x1 = a;
        y1 = b;
        x2 = c;
        y2 = d;
        //brush = e;
        color = f;
    }
    void Drag(int move_x, int move_y)//move the pattern
    {
        x1 += move_x;
        y1 += move_y;
        x2 += move_x;
        y2 += move_y;
    }
    abstract void ReShape(int sizeup);
    void BrushChange(int brushup)
    {
        brush += brushup;
        if(brush < 0) brush = 1;
    }
    void ColorChange(int new_color)
    {
        color = new_color;
    }
    void Set(Graphics g)
    {
        switch(color)
        {
            case 0: {g.setColor(Color.WHITE); break;}
            case 1: {g.setColor(Color.BLACK); break;}
            case 2: {g.setColor(Color.RED); break;}
            case 3: {g.setColor(Color.GREEN); break;}
            case 4: {g.setColor(Color.YELLOW); break;}
            case 5: {g.setColor(Color.BLUE); break;}
            default: {g.setColor(Color.BLACK); break;}
        }
        Graphics2D g2d = (Graphics2D) g;
        BasicStroke s =  new BasicStroke(brush);
        g2d.setStroke(s);
    }
    void DrawRange(Graphics g)//draw the outline
    {
        g.setColor(Color.GRAY);
        Graphics2D g2d = (Graphics2D) g;
        g2d.setStroke(new BasicStroke(5));
        g.drawLine(x1, y1, x1, y1);
        g.drawLine(x1, y2, x1, y2);
        g.drawLine(x2, y1, x2, y1);
        g.drawLine(x2, y2, x2, y2);
    }
    abstract void repaint(Graphics g);
}
class DrawLine extends DrawUnit{
    public DrawLine(int a, int b, int c, int d, int f)
    {
        super(a, b, c, d, f);
    }
    public void repaint(Graphics g)
    {    
        //System.out.println("DrawLine.Draw()"+"("+x1+ ","+y1+") to ("+x2+","+y2+")  "+color+" <-color  brush->"+brush);
        g.drawLine(x1, y1, x2, y2);
    }
    public void ReShape(int sizeup)
    {
        x2 += ((x2 - x1) * 0.1 * sizeup);
        y2 += ((y2 - y1) * 0.1 * sizeup);
    }
}
class DrawRect extends DrawUnit{

    public DrawRect(int a, int b, int c, int d, int f)
    {
        super(a, b, c, d, f);
    }
    public void repaint(Graphics g)
    {    
        //System.out.println("DrawRect.Draw()"+"("+x1+ ","+y1+") to ("+x2+","+y2+")  "+color+" <-color  brush->"+brush);
        int xx = x1<x2?x1:x2;
        int yy = y1<y2?y1:y2;
        g.drawRect(xx, yy, Math.abs(x2-x1), Math.abs(y2-y1));
    }
    public void ReShape(int sizeup)
    {
        x2 += ((x2 - x1) * 0.1 * sizeup);
        y2 += ((y2 - y1) * 0.1 * sizeup);
    }
}
class DrawCircle extends DrawUnit{

    public DrawCircle(int a, int b, int c, int d, int f)
    {
        super(a, b, c, d, f);
    }
    public void repaint(Graphics g)
    {    
        int xx = x1<x2?x1:x2;
        int yy = y1<y2?y1:y2;
        //System.out.println("DrawCircle.Draw()"+"("+x1+ ","+y1+") to ("+x2+","+y2+")  "+color+" <-color  brush->"+brush);
        g.drawOval(xx, yy, Math.abs(x2-x1), Math.abs(y2-y1));
    }
    public void ReShape(int sizeup)
    {
        x2 += ((x2 - x1) * 0.1 * sizeup);
        y2 += ((y2 - y1) * 0.1 * sizeup);
    }
}
class DrawText extends DrawUnit{
    String text;
    Font f = null;
    public DrawText(int a, int b, int c, int d, int f, String s)
    {
        super(a, b, c, d, f);
        text = s;
    }
    public void repaint(Graphics g)
    {
        int xx = x1<x2?x1:x2;
        int yy = y1<y2?y1:y2;
        System.out.println("DrawText :" + text +"    ("+ xx + ","+yy+")");
        if(f == null) f = g.getFont();//initialize the font
        else g.setFont(f);
        g.drawString(text, xx, yy + Math.abs(y2-y1)/2);
    }
    public void ReShape(int sizeup)
    {
        if(f == null) return;
        Font newf = new Font(f.getName(), f.getStyle(), f.getSize() + sizeup);//create new font
        f = newf;
    }
}