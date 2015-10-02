using System;
using System.Net.Sockets;
using System.Text;
using System.Net;
using System.Drawing;
using System.Drawing.Imaging;
using System.Drawing.Drawing2D;
using System.IO;
using System.Threading;

namespace TcpServer
{
	class MainClass {
		public static void Main(string[] args) {
			Console.WriteLine("Hello World!");

			TcpListener server = new TcpListener(IPAddress.Any, 9999);  
			// we set our IP address as server's address, and we also set the port: 9999

			server.Start();  // this will start the server


			while (true)   //we wait for a connection
			{
				using (TcpClient client = server.AcceptTcpClient()) {

					using (NetworkStream ns = client.GetStream()) { //networkstream is used to send/receive messages

						var img = Directory.GetFiles("/home/leo/teensy/SerialIP/foto", "*.JPG"); 

						/*
						byte[] hello = new byte[100];   //any message must be serialized (converted to byte array)
						hello = Encoding.Default.GetBytes("Ciao\n");  //conversion string => byte array

						ns.Write(hello, 0, hello.Length);     //sending the message

						while (client.Connected) {  //while the client is connected, we look for incoming messages
							byte[] msg = new byte[1024];     //the messages arrive as byte array
							int len = ns.Read(msg, 0, msg.Length);   //the same networkstream reads the message sent by the client
							if (len == 0)
								break;

							string s = Encoding.Default.GetString(msg, 0, len);
							if (s != "") Console.WriteLine(s); //now , we write the message as string
						}
						*/

						var b = new byte[2];
						ns.Read(b, 0, 2);

						int imgIndex = ToUShort(b);

						string ss = img[imgIndex % img.Length];
						readImage(ss ,ns);

						client.Close();

						Console.WriteLine("Connessione chiusa");
					}
				}
			}
		}

		static ushort ToUShort(byte [] b) { return (ushort)((b[0] << 0) | (b[1] << 8)); }

		static ushort color565(int r, int g, int b) { return (ushort)( (((uint)r & 0xF8) << 8) | (((uint)g & 0xFC) << 3) | ((uint)b >> 3)); }


		public static Bitmap resizeImage(int newWidth, int newHeight, string stPhotoPath)
		{
			Image imgPhoto = Image.FromFile(stPhotoPath); 

			int sourceWidth = imgPhoto.Width;
			int sourceHeight = imgPhoto.Height;

			//Consider vertical pics
			if (sourceWidth < sourceHeight)
			{
				int buff = newWidth;

				newWidth = newHeight;
				newHeight = buff;
			}

			int sourceX = 0, sourceY = 0, destX = 0, destY = 0;
			float nPercent = 0, nPercentW = 0, nPercentH = 0;

			nPercentW = ((float)newWidth / (float)sourceWidth);
			nPercentH = ((float)newHeight / (float)sourceHeight);
			if (nPercentH < nPercentW)
			{
				nPercent = nPercentH;
				destX = System.Convert.ToInt16((newWidth - (sourceWidth * nPercent)) / 2);
			}
			else
			{
				nPercent = nPercentW;
				destY = System.Convert.ToInt16((newHeight - (sourceHeight * nPercent)) / 2);
			}

			int destWidth = (int)(sourceWidth * nPercent);
			int destHeight = (int)(sourceHeight * nPercent);


			Bitmap bmPhoto = new Bitmap(newWidth, newHeight, PixelFormat.Format24bppRgb);

			//bmPhoto.SetResolution(imgPhoto.HorizontalResolution, imgPhoto.VerticalResolution);

			Graphics grPhoto = Graphics.FromImage(bmPhoto);
			grPhoto.Clear(Color.Black);
			grPhoto.InterpolationMode = InterpolationMode.HighQualityBicubic;

			grPhoto.DrawImage(imgPhoto,
				new Rectangle(destX, destY, destWidth, destHeight),
				new Rectangle(sourceX, sourceY, sourceWidth, sourceHeight),
				GraphicsUnit.Pixel);

			grPhoto.Dispose();
			imgPhoto.Dispose();
			return bmPhoto;
		}
		static void readImage(string s, NetworkStream ns)
		{
			using (var image = resizeImage(320, 240, s)) {
				int w = image.Width;
				int h = image.Height;

				byte[] b = new byte[2];
				b[0] = (byte)(w & 0xff);
				b[1] = (byte)(w >> 8);
				ns.Write(b, 0, 2);

				b[0] = (byte)(h & 0xff);
				b[1] = (byte)(h >> 8);
				ns.Write(b, 0, 2);
				ns.Flush();

				for (int y = 0; y < h; y++) {

					b = new byte[w * 2];
					int i = 0;
					for (int x = 0; x < w; x++) {
						var col = image.GetPixel(x, y);
						var rgb = color565(col.R, col.G, col.B);
						b[i++] = (byte)(rgb & 0xff);
						b[i++] = (byte)(rgb >> 8);
					}
					ns.Write(b, 0, b.Length);

					byte ck = 0;
					foreach (var c in b) ck ^= c;

					ns.Read(b, 0, 1);
					if (ck != b[0])
						break;


					ns.Read(b, 0, 2);
					if (y == ToUShort(b))
						continue;
					else
						break;
				}
			}
		}
	}
}
