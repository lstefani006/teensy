using System;
using System.IO.Ports;
using System.Threading;
using System.Net;
using System.Net.Sockets;
using System.Threading.Tasks;
using System.Collections.Generic;

class MainClass 
{


	static void Main() 
	{
		try 
		{
			for (;;)
			{
				string port = null;
				SerialPort s = null;
				{
					var pp = new List<string>();
					for (int p = 0; p < 10; ++p)
						pp.Add(string.Format("/dev/ttyACM{0}", p));
					for (int p = 0; p < 10; ++p)
						pp.Add(string.Format("/dev/ttyUSB{0}", p));

					bool ok = false;
					do 
					{
						for (int p = 0; p < pp.Count; ++p)
						{
							try
							{
								port = pp[p];
								s = new SerialPort(port);
								//s.BaudRate = 19200;
								//s.BaudRate = 115200;
								s.BaudRate = 38400;
								s.DataBits = 8;
								s.Parity = Parity.None;
								s.StopBits = StopBits.One;
								s.Handshake = Handshake.None;
								s.DtrEnable = false;   // cosi non resetta
								s.ReadTimeout = -1;
								s.Open();

								ok = true;
								break;
							}
							catch (Exception)
							{
							}
						}
					}
					while(ok == false);
				}

				Console.WriteLine("Port open on {0}", port);

				for (;;)
				{
					int b = s.ReadByte();
					if (b == '7')
					{
						var n = (DateTime.Now - new DateTime (1970, 1, 1)).TotalSeconds;
						var str = String.Format("T{0} ", n);

						s.Write(str);
						Console.WriteLine("Sync done");
					}
				}
			}
		}
		catch (Exception ex)
		{
			Console.WriteLine("Task.Run error. {0}", ex);
		}
	}

}
