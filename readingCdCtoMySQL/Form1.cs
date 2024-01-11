using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using MySql.Data.MySqlClient;

namespace readingcdcPrj
{
    public partial class Form1 : Form
    {
        string Conn = "Server=localhost;Database=example;Uid=root;Pwd=qwer1234;";
        public Form1()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            if(textBox1.Text == "")
            {
                MessageBox.Show("포트 번호를 입력하세요~");
            }
            else
            {
                serialPort1.PortName = textBox1.Text;
                if(serialPort1.IsOpen == false)
                {
                    serialPort1.Open();
                }
            }
        }

        private void button2_Click(object sender, EventArgs e)
        {
            if (serialPort1.IsOpen == true)
            {
                serialPort1.Close();
            }
        }

        private void serialPort1_DataReceived(object sender, System.IO.Ports.SerialDataReceivedEventArgs e)
        {
            string data = serialPort1.ReadLine();
            string date = DateTime.Now.ToString();
            label4.Text = "조도센서 데이터 : " + data;

            using (MySqlConnection conn = new MySqlConnection(Conn))
            {
                conn.Open();
                MySqlCommand msc = new MySqlCommand(
                    "insert into example1(data, date) values(" + data + ", '" + date + "')"
                    , conn);
                msc.ExecuteNonQuery();
            }



        }

        private void button3_Click(object sender, EventArgs e)
        {
            using (MySqlConnection conn = new MySqlConnection(Conn))
            {
                DataSet ds = new DataSet();
                string sql = "select * from example1 order by date desc limit 10";
                MySqlDataAdapter adpt = new MySqlDataAdapter(sql, conn);
                adpt.Fill(ds, "example1");

                for(int i = 0; ds.Tables[0].Rows.Count > i; i++)
                {
                    ListViewItem lvi = new ListViewItem();
                    lvi.Text = ds.Tables[0].Rows[i]["num"].ToString();

                    lvi.SubItems.Add(ds.Tables[0].Rows[i]["data"].ToString());
                    lvi.SubItems.Add(ds.Tables[0].Rows[i]["date"].ToString());

                    listView1.Items.Add(lvi);
                }
            }
        }

        private void button4_Click(object sender, EventArgs e)
        {
            using (MySqlConnection conn = new MySqlConnection(Conn))
            {
                conn.Open();
                MySqlCommand msc = new MySqlCommand("DELETE FROM example1", conn);
                msc.ExecuteNonQuery();
            }
            listView1.Items.Clear();
        }
    }
}
