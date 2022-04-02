using System;
using System.Data;
using System.Linq;
using System.Windows.Forms;
using System.IO.Ports;
using System.Drawing;
using System.Threading;
using System.Threading.Tasks;

namespace WindowsFormsApplication1
{
    public partial class Form1 : Form
    {
        private int algorithmCount = 0;
        private int algorithmNumber;

        public Form1()
        {
            InitializeComponent();
        }
        
        private void comboBox1_Click(object sender, EventArgs e)
        {
            int num;
            comboBox1.Items.Clear();
            string[] ports = SerialPort.GetPortNames().OrderBy(a => a.Length > 3 && int.TryParse(a.Substring(3), out num) ? num : 0).ToArray();
            comboBox1.Items.AddRange(ports);
        }

        private void buttonOpenPort_Click(object sender, EventArgs e)
        {
            if (!serialPort1.IsOpen)
                try
                {
                    serialPort1.PortName = comboBox1.Text;
                    serialPort1.Open();
                    buttonOpenPort.Text = "Close";
                    comboBox1.Enabled = false;
                    button1.Visible = true;
                    button2.Visible = true;
                    panel1.Visible = true;
                    panel2.Visible = true;
                    panel3.Visible = true;
                    panel4.Visible = true;
                    panel5.Visible = true;
                    panel6.Visible = true;
                    panel7.Visible = true;
                    panel8.Visible = true;
                }
                catch
                {
                    MessageBox.Show("Port " + comboBox1.Text + " is invalid!", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
            else
            {
                serialPort1.Close();
                buttonOpenPort.Text = "Open";
                comboBox1.Enabled = true;
                button1.Visible = false;
                button2.Visible = false;
                panel1.Visible = false;
                panel2.Visible = false;
                panel3.Visible = false;
                panel4.Visible = false;
                panel5.Visible = false;
                panel6.Visible = false;
                panel7.Visible = false;
                panel8.Visible = false;
            }
        }

        private void button1_Click(object sender, EventArgs e)
        {
            serialPort1.Write("1");
        }

        private void button2_Click(object sender, EventArgs e)
        {
            serialPort1.Write("2");
        }

        private void clearAllLed()
        {
            panel1.BackColor = Color.Black;
            panel2.BackColor = Color.Black;
            panel3.BackColor = Color.Black;
            panel4.BackColor = Color.Black;
            panel5.BackColor = Color.Black;
            panel6.BackColor = Color.Black;
            panel7.BackColor = Color.Black;
            panel8.BackColor = Color.Black;
        }

        private void startTimer()
        {
            timer1.Start();
        }

        private async void timer1_Tick(object sender, EventArgs e)
        {
            Panel[] panels = new Panel[8] { panel1, panel2, panel3, panel4, panel5, panel6, panel7, panel8 };
            Color[] panelColors = new Color[8] { Color.Firebrick, Color.Blue, Color.Lime, Color.Firebrick, Color.Lime, Color.Blue, Color.Firebrick, Color.Lime };
            algorithmCount++;
            if (algorithmCount < 2)
               if (algorithmCount % 2 == 1)
              {
                    if (algorithmNumber == 1)
                    {
                        for (int i = 0; i < panels.Length / 2; i++)
                        {
                            panels[i].BackColor = panelColors[i];
                            await Task.Delay(700);
                            panels[i].BackColor = Color.Black;

                            panels[panels.Length - 1 - i].BackColor = panelColors[panels.Length - 1 - i];
                            await Task.Delay(700);
                            panels[panels.Length - 1 - i].BackColor = Color.Black;
                        }
                        clearAllLed();
                    }
                    else if (algorithmNumber == 2)
                    {
                        for (int i = 0; i < panels.Length; i++)
                        {
                            panels[i].BackColor = panelColors[i];
                            await Task.Delay(700);
                            panels[i].BackColor = Color.Black;
                        }
                        clearAllLed();
                    }

                }
        }

        private void serialPort1_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            char commandFromArduino = (char)serialPort1.ReadChar();
            if (commandFromArduino == '1')
            {
                algorithmNumber = 1;
                algorithmCount = 0;
                this.BeginInvoke(new ThreadStart(startTimer));
            
            }
             if (commandFromArduino == '2')
            {
                algorithmNumber = 2;
                algorithmCount = 0;
                this.BeginInvoke(new ThreadStart(startTimer));
            }
        }

        private void Form1_Load(object sender, EventArgs e)
        {
        }

        private void comboBox1_SelectedIndexChanged(object sender, EventArgs e)
        {
        }
    }
}
