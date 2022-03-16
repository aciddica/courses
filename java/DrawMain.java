import javax.swing.*;
import java.awt.Color;
import java.awt.event.*;
import java.util.ArrayList;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.BasicStroke;
public class DrawMain extends JFrame//窗口
{
    MyPanel mydraw = null;
    public DrawMain()
    {
        this.setVisible(true);
        this.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        this.setSize(800, 500);
        this.setLocation(200, 100);
        mydraw = new MyPanel();
        this.add(mydraw);

    }
    public static void main(String[] args) {
        DrawMain main = new DrawMain();
    }

}
class Strip//每一个小线段
{
    int x1, x2, y1, y2;
    int color, width;
    public Strip(int a, int b, int c, int d, int e, double f)//获取位置信息与笔刷粗细
    {
        x1 = a;
        y1 = b;
        x2 = c;
        y2 = d;
        color = e;
        width = (int)(f);
    }
    public void repaint(Graphics g)//绘制每一小段
    {
        if(color == 0) g.setColor(Color.WHITE);//根据左右键选择颜色
        else g.setColor(Color.BLACK);
        Graphics2D g2d = (Graphics2D) g;
        BasicStroke s =  new BasicStroke(width);//设置笔刷粗细
        g2d.setStroke(s);
        if(width > 10) width = 1;
        g.drawLine(x1, y1, x2, y2);
    }
}
class MyPanel extends JPanel implements MouseMotionListener, MouseListener//检测鼠标行为，进行整体绘制
{

    int last_x, last_y, cur_x, cur_y, color, width;
    
    ArrayList<Strip> strips = new ArrayList<Strip>();
    Graphics g;
    public MyPanel()
    {
        this.addMouseListener(this);
        this.addMouseMotionListener(this);
    }
    public void paint(Graphics g)
    {
        super.paint(g);
        for(int i = 0; i < strips.size(); i++)
        {
            strips.get(i).repaint(g);
        }
    }

    public void mouseDragged(MouseEvent e)//拖动鼠标时进行绘制
    {   
        int dist = (cur_x - last_x)*(cur_x - last_x) + (cur_y - last_y)*(cur_y - last_y);     
        last_x = cur_x;
        last_y = cur_y;
        cur_x = e.getX();
        cur_y = e.getY();
        
        strips.add(new Strip(last_x, last_y, cur_x, cur_y, color, 2.0/dist ));
        this.repaint();

    }
    public void mouseMoved(MouseEvent e)
    {

    }
    public void mouseReleased(MouseEvent e)
    {

        
    }
    public void mouseEntered(MouseEvent e)
    {

    }
    public void mousePressed(MouseEvent e) //按下鼠标时检测颜色
    {
        if(e.getButton() == 1) //左键
            color = 1;
        else if(e.getButton() == 3) //右键
            color = 0;
        else color = 2;
        cur_x = e.getX();
        cur_y = e.getY();
        last_x = e.getX();
        last_y = e.getY();
        this.repaint();
    }
    public void mouseExited(MouseEvent e)
    {

    }
    public void mouseClicked(MouseEvent e)
    {

    }
}
