using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using MySql.Data.MySqlClient; //MySQL >> 추가해야 할 부분

namespace mysqlcs1
{
    public partial class Form1 : Form
    {
        string Conn = "Server=localhost;Database=example;Uid=root;Pwd=qwer1234;"; //MySQL >> 추가해야 할 부분

        public Form1()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            if(textBox1.Text == "" || textBox2.Text == "")
            {
                MessageBox.Show("도시 또는 인구수를 입력해주세요 !");
            }
            else
            {
                //DB 데이터 삽입
                //using을 쓰면 알아서 MySQL과 disconnect를 해준다.
                using (MySqlConnection conn = new MySqlConnection(Conn)) 
                {
                    conn.Open(); //접속 
                    MySqlCommand msc = new MySqlCommand(
                        "insert into example1(city,pop) values('"+textBox1.Text+"','"+textBox2.Text+"')"
                        , conn);
                    int res = msc.ExecuteNonQuery(); //SQL 쿼리가 MySQL로 날아간다.
                    
                    if(res == 1) {
                        MessageBox.Show("도시와 인구수가 제대로 입력되었습니다!");
                    }


                }
            }
        }

        private void button2_Click(object sender, EventArgs e)
        {
            using (MySqlConnection conn = new MySqlConnection(Conn))
            {
                DataSet ds = new DataSet(); //시트가 날라오면 DataSet의 자료형과 정확히 일치
                string sql = "SELECT * FROM  example1";
                MySqlDataAdapter adpt = new MySqlDataAdapter(sql, conn);
                adpt.Fill(ds, "example1");

                //받아온 테이블 전체를 순회하는 과정
                for(int i = 0;i<ds.Tables[0].Rows.Count;i++)
                {
                    //한 레코드씩 리스트뷰에 집어넣는 과정 
                    ListViewItem lvi = new ListViewItem();
                    lvi.Text = ds.Tables[0].Rows[i]["num"].ToString();

                    lvi.SubItems.Add(ds.Tables[0].Rows[i]["city"].ToString());
                    lvi.SubItems.Add(ds.Tables[0].Rows[i]["pop"].ToString());

                    listView1.Items.Add(lvi);
                }
            }
        }

        private void button3_Click(object sender, EventArgs e)
        {
            listView1.Items.Clear();
        }
    }
}
