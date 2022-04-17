using System;
using System.Linq;
using System.Windows.Forms;
using System.IO.Ports;
using System.Drawing;
using System.Threading;

namespace WindowsFormsApplication1
{
    public partial class Form1 : Form
    {
        private int _algorithmNumber;
        private int _numberForDelay;

        public Form1()
        {
            InitializeComponent();
        }
        
        private void comboBox1_Click(object sender, EventArgs e)
        {
            comboBox1.Items.Clear();
            var ports = SerialPort.GetPortNames().OrderBy(
                a => a.Length > 3 && 
                     int.TryParse(a.Substring(3), out var num) ? num : 0)
                .ToArray();
            comboBox1.Items.AddRange(ports);
        }

        private void buttonOpenPort_Click(object sender, EventArgs e) 
        {
            if (!serialPort1.IsOpen)
                try
                {
                    serialPort1.PortName = comboBox1.Text;
                    serialPort1.Open();
                    buttonOpenPort.Text = @"Close";
                    comboBox1.Enabled = false;
                    button1.Visible = true;
                    button2.Visible = true;
                }
                catch
                {
                    MessageBox.Show(
                        $@"Port {comboBox1.Text} is invalid!", 
                        @"Error",
                        MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
            else
            {
                serialPort1.Close();
                buttonOpenPort.Text = @"Open";
                comboBox1.Enabled = true;
                button1.Visible = false;
                button2.Visible = false;
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

        private void ClearAllLed()
        {
            panel1.BackColor = Color.SkyBlue;
            panel2.BackColor = Color.SkyBlue;
            panel3.BackColor = Color.SkyBlue;
            panel4.BackColor = Color.SkyBlue;
            panel5.BackColor = Color.SkyBlue;
            panel6.BackColor = Color.SkyBlue;
            panel7.BackColor = Color.SkyBlue;
            panel8.BackColor = Color.SkyBlue;
        }

        private void StartTimer()
        {
            timer1.Stop();
            timer2.Stop();
            switch (_algorithmNumber)
            {
                case 1:
                    timer1.Start();
                    break;
                case 2:
                    timer2.Start();
                    break;
            }
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            ClearAllLed();
            Panel[] blinkAlgorithm1 = { panel1, panel8, panel2, panel7, panel3, panel6, panel4, panel5 };
            
            if (_numberForDelay >= 8 || _algorithmNumber != 1) 
                return;
            
            blinkAlgorithm1[_numberForDelay].BackColor = Color.Red;
            _numberForDelay++;
            blinkAlgorithm1[_numberForDelay].BackColor = Color.Red;
            _numberForDelay++;

        }

        private void timer2_Tick(object sender, EventArgs e)
        {
            ClearAllLed();
            Panel[] blinkAlgorithm2 = { panel8, panel6, panel4, panel2, panel7, panel5, panel3, panel1 };

            if (_numberForDelay >= 8 || _algorithmNumber != 2) 
                return;
            
            blinkAlgorithm2[_numberForDelay].BackColor = Color.Red;
            _numberForDelay++;
        }

        private void serialPort1_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            var commandFromArduino = (char)serialPort1.ReadChar();
            switch (commandFromArduino)
            {
                case '1':
                    _algorithmNumber = 1;
                    _numberForDelay = 0;
                    BeginInvoke(new ThreadStart(StartTimer));
                    break;
                
                case '2':
                    _algorithmNumber = 2;
                    _numberForDelay = 0;
                    BeginInvoke(new ThreadStart(StartTimer));
                    break;
            }
        }
    }
}
