using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Google.Protobuf.WellKnownTypes;
using MySql.Data.MySqlClient;
using MySqlX.XDevAPI.Relational;


namespace MachineLearningTest
{

    internal class Program
    {
        // rainfall  = (date.Month * w_6) + (date.Day * w_5) + (date.Hour * w_4)
        // + (temp * w_3) + (humid * w_2) + (light * w_1) + w_0

        static double w_0 = 0.0;
        static double w_1 = 0.0;
        static double w_2 = 0.0;   
        static double w_3 = 0.0;
        static double w_4 = 0.0;
        static double w_5 = 0.0;
        static double w_6 = 0.0;
        static double progress_rate = 0.0;
        const int batchSize = 2000;
        const double learningRate = 0.000303;
        const double start = 1.0;
        const double end = 522761.0;
        
        static void Main(string[] args)
        {
            string Conn = "Server=localhost;Database=example;Uid=root;Pwd=qwer1234;";

            System.Console.Title = "날씨 데이터 학습중 ...";
            System.Console.ForegroundColor = ConsoleColor.Red;

            using (MySqlConnection conn = new MySqlConnection(Conn))
            {
                //datetime()호출
                DataSet ds = new DataSet();
                string sql = "SELECT * FROM weather_raw WHERE num BETWEEN " + start + " AND " + end + "";
                MySqlDataAdapter adpt = new MySqlDataAdapter(sql, conn);
                adpt.Fill(ds, "weather_raw");

                DataTable dt = ds.Tables["weather_raw"];

                int p = 1;
                for(int i = 0; i < dt.Rows.Count; i += batchSize)
                {
                    int startIndex = i;
                    int availableBatchSize = Math.Min(batchSize,dt.Rows.Count-i);

                    DataRow[] currentBatch = new DataRow[availableBatchSize];
                    
                    DataRow[] allRows = dt.Rows.Cast<DataRow>().ToArray();
                    var filteredRows = allRows.Skip(startIndex).Take(availableBatchSize).Where(row => row != null).ToArray();

                    Array.Copy(filteredRows, 0, currentBatch, 0,availableBatchSize);

                    batchGradientDescent(currentBatch);

                    progress_rate = ((p++ * batchSize) / end * 100);

                    int left = Console.CursorLeft;
                    int top = Console.CursorTop;

                    // 출력하려는 문자열 생성
                    string output = $"잠시만 기다려 주세요 {progress_rate:F2}%";

                    // 이전 출력 내용 지우기
                    Console.SetCursorPosition(left, top);
                    Console.Write(output);
                    Console.SetCursorPosition(left, top);

                    //Console.WriteLine($"progress :{progress_rate:F2}%   >>  w_0 :{w_0:F3}, w_1 :{w_1:F3}, w_2 :{w_2:F3}, w_3 :{w_3:F3}, w_4 :{w_4:F3}," +
                    //    $" w_5 :{w_5:F3}, w_6 :{w_6:F3}");
                }



                //// 테이블 개수 출력
                //Console.WriteLine($"Number of tables in DataSet: {ds.Tables.Count}");
                //// 출력을 위한 메서드 호출
                //PrintDataSet(ds);

                //DataTable weatherraw = ds.Tables[0];
                //foreach (DataRow row in weatherraw.Rows)
                //{
                //    Console.WriteLine(row[0].);
                //}
                //foreach (DataRow row in weatherraw.Rows)
                //{
                //    DateTime date = (DateTime)row["date"];
                //    int month = date.Month;
                //    int day = date.Day;
                //    int hour = date.Hour;
                //    int minute = date.Minute;
                //    Console.WriteLine($"Month:{month},Day: {day} - Hour:{hour},Minute:{minute}");
                //}

            }

            using (MySqlConnection conn = new MySqlConnection(Conn))
            {
                conn.Open();
                MySqlCommand msc = new MySqlCommand("DELETE FROM machinelearningresult", conn);
                msc.ExecuteNonQuery();
            }

            using (MySqlConnection conn = new MySqlConnection(Conn))
            {
                conn.Open();
                MySqlCommand msc = new MySqlCommand("INSERT INTO machinelearningresult(w_0,w_1,w_2,w_3,w_4,w_5,w_6)" +
                    " values("+w_0+ ","+w_1+","+w_2+","+w_3+","+w_4+","+w_5+ ","+w_6+")", conn);
                msc.ExecuteNonQuery();
            }

        }

        static void batchGradientDescent(DataRow[] currentBatch)
        {
            //w_0 calculation 
            double sumOfErr_w0 = 0.0;
            double sumOfErr_w1 = 0.0;
            double sumOfErr_w2 = 0.0;
            double sumOfErr_w3 = 0.0;
            //double sumOfErr_w4 = 0.0;
            //double sumOfErr_w5 = 0.0;
            //double sumOfErr_w6 = 0.0;

            // rainfall  = (date.Month * w_6) + (date.Day * w_5) + (date.Hour * w_4)
            // + (temp * w_3) + (humid * w_2) + (light * w_1) + w_0

            foreach (DataRow row in currentBatch)
            {

                //DateTime date = (DateTime)row["date"];
                //double month = date.Month;
                //double day = date.Day;
                //double hour = date.Hour;

                double rainfall = (double)row["rainfall"] ;
                double light = (double)row["light"]  ;
                double humid = (double)row["humid"] ;
                double temp = (double)row["temp"] ;

                sumOfErr_w0 += rainfall - (w_0 + (w_1 * light) + (w_2 * humid) + (w_3 * temp));
                //+ (w_4 * hour) + (w_5 * day) + (w_6 * month));

                sumOfErr_w1 += rainfall - (w_0 + (w_1 * light) + (w_2 * humid) + (w_3 * temp))*light;
                //+ (w_4 * hour) + (w_5 * day) + (w_6 * month)))* light;

                sumOfErr_w2 += rainfall - (w_0 + (w_1 * light) + (w_2 * humid) + (w_3 * temp)) * humid;
                //+ (w_4 * hour) + (w_5 * day) + (w_6 * month))) * humid;

                sumOfErr_w3 += rainfall - (w_0 + (w_1 * light) + (w_2 * humid) + (w_3 * temp)) * temp;
                            //+ (w_4 * hour) + (w_5 * day) + (w_6 * month))) * temp;

                //sumOfErr_w4 += (rainfall - (w_0 + (w_1 * light) + (w_2 * humid) + (w_3 * temp)
                //            + (w_4 * hour) + (w_5 * day) + (w_6 * month))) * hour;

                //sumOfErr_w5 += (rainfall - (w_0 + (w_1 * light) + (w_2 * humid) + (w_3 * temp)
                //            + (w_4 * hour) + (w_5 * day) + (w_6 * month))) * day;

                //sumOfErr_w6 += (rainfall - (w_0 + (w_1 * light) + (w_2 * humid) + (w_3 * temp)
                //            + (w_4 * hour) + (w_5 * day) + (w_6 * month))) * month;

            }

            w_0 += learningRate * (sumOfErr_w0 / batchSize);
            w_1 += learningRate * (sumOfErr_w1 / batchSize);
            w_2 += learningRate * (sumOfErr_w2 / batchSize);
            w_3 += learningRate * (sumOfErr_w3 / batchSize);
            //w_4 += learningRate * sumOfErr_w4;
            //w_5 += learningRate * sumOfErr_w5;
            //w_6 += learningRate * sumOfErr_w6;

            //foreach (DataRow row in currentBatch)
            //{

            //    Console.WriteLine($"Value: {row["temp"]}");

            //    // 실제 데이터 타입 확인
            //    System.Type subType = row["temp"].GetType();
            //    Console.WriteLine($"Type: {subType}");

            //    // 추가적인 처리 로직을 여기에 추가
            //}

        }


        //static void PrintDataSet(DataSet dataSet)
        //{
        //    // DataSet에 포함된 각 DataTable에 대해 반복
        //    foreach (DataTable table in dataSet.Tables)
        //    {
        //        Console.WriteLine($"Table: {table.TableName}");

        //        // 각 행에 대해 반복
        //        foreach (DataRow row in table.Rows)
        //        {
        //            // 각 열에 대해 반복
        //            foreach (DataColumn column in table.Columns)
        //            {
        //                Console.Write($"{column.ColumnName}: {row[column]}   ");
        //            }
        //            Console.WriteLine(); // 각 행이 끝날 때마다 줄바꿈
        //        }
        //        Console.WriteLine(); // 각 DataTable이 끝날 때마다 줄바꿈
        //    }
        //}
    }
}
