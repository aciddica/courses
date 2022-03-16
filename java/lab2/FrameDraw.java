package lab2;
import javax.swing.*;
import java.awt.event.*;

public class FrameDraw extends JFrame// implements MouseListener
{
    private DrawPanel panel=null;

    public FrameDraw(){
        panel=new DrawPanel();

        this.add(panel);

        this.setVisible(true);

        this.setSize(300,300);

}

public static void main(String[] args)

{
        new FrameDraw();

}

}
