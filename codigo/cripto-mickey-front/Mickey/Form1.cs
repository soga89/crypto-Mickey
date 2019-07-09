using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO;
using System.Diagnostics;


namespace Mickey
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
            this.FormBorderStyle = FormBorderStyle.FixedDialog;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
        }

        private void Form1_Load(object sender, EventArgs e)
        {

        }

        private void Button1_Click(object sender, EventArgs e)
        {
            if(ruta.Text != "")
            {
                System.Diagnostics.Process process1 = new System.Diagnostics.Process();
                process1.StartInfo.UseShellExecute = true;
                process1.StartInfo.FileName = "cypher.exe";
                process1.StartInfo.Arguments = this.ruta.Text;
                process1.Start();
                process1.WaitForExit();
                if (process1.ExitCode == 0)
                {
                    System.Windows.Forms.MessageBox.Show("LA IMAGEN SE PROCESO CORRECTAMENTE", "OK", System.Windows.Forms.MessageBoxButtons.OK);
                }
                else {
                    System.Windows.Forms.MessageBox.Show("HUBO UN PROBLEMA AL PROCESAR LA IMAGEN", "ERROR", System.Windows.Forms.MessageBoxButtons.OK);
                }
                process1.Close();
            }

        }

   
        private void Buscar_Click(object sender, EventArgs e)
        {
            OpenFileDialog openFileDialog1 = new OpenFileDialog
            {
                InitialDirectory = @"C:\",

                CheckFileExists = true,
                CheckPathExists = true,
                DefaultExt = "bmp",
                Filter = "bmp files (*.bmp)|*.bmp|All files (*.*)|*.*",
                RestoreDirectory = true,

                ReadOnlyChecked = true,
                ShowReadOnly = true
            };

            if (openFileDialog1.ShowDialog() == DialogResult.OK)
            {
                ruta.Text = openFileDialog1.FileName;
                this.pictureBox1_Click(sender, e);
            }
        }

        private void pictureBox1_Click(object sender, EventArgs e)
        {
            if (this.ruta.Text != "") {
                this.pictureBox1.Image = Image.FromFile(this.ruta.Text);
                this.pictureBox1.SizeMode = PictureBoxSizeMode.Zoom;
                this.Controls.Add(this.pictureBox1);
            }
        }
    }
}
