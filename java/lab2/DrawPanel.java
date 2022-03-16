package lab2;
import javax.swing.*;

import java.awt.*;

import java.awt.event.*;

public class DrawPanel extends JPanel implements MouseListener

{
    static int x=0,x1=0,y=0,y1=0;

    public DrawPanel(){
    this.addMouseListener(this);

}

        public void paint(Graphics g){
        super.paint(g);

        //Image image=Toolkit.getDefaultToolkit().getImage("images/psu.jpg");

        //g.drawImage(image,0,0,300,300,this);

        g.drawLine(x,y,x1,y1);

}

public void mouseClicked(MouseEvent e){
//获取起点坐标

if(e.getButton()==MouseEvent.BUTTON3){
x=e.getX();

y=e.getY();

//label.setText("鼠标点击的位置 启动坐标:"+" "+"x...."+x+"y...."+y);

System.out.println("x..."+x+"...."+y);

}

//获取终点坐标

if(e.getButton()==MouseEvent.BUTTON1){
x1=e.getX();

y1=e.getY();

System.out.println("终点的坐标..."+x1+"..."+y1);

//当左击的时候，进行连线

if(y>=0||x>=0)

{
this.repaint();

this.validate();

}

else

System.out.println("数据出错。。。");

}

}

public void mouseEntered(MouseEvent e){
}

public void mouseExited(MouseEvent e){
}

public void mousePressed(MouseEvent e){
}

public void mouseReleased(MouseEvent e){
}

public static void main(String[] args) {
new DrawPanel();

}

}
