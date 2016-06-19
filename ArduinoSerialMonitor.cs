using System;
using System.IO.Ports;
using System.Threading;
using System.Net;
using System.Net.Sockets;
using System.Threading.Tasks;
using System.Collections.Generic;

namespace ArduionoSerialMonitor
{
	class MainClass {

		SerialPort _s;
		volatile char __cmd;

		const int cPort = 53557;

		char Cmd {
			get {
				lock (this)
					return __cmd;
			}
			set {
				lock (this)
					__cmd = value;
			}
		}

		void begin() {

			this.Cmd = 'R';

			Task.Run(async () => {
				try {
					var c = new UdpClient(cPort);
					for (;;) {
						var r = await c.ReceiveAsync();
						if (r.Buffer.Length > 0)
						{
							if (r.Buffer[0] == 'S')
							this.Cmd = 'S';
							else if (r.Buffer[0] == 'R')
							this.Cmd = 'R';
							else
							this.Cmd = '?';
						}
					}
				}
				catch (Exception ex) {
					Console.WriteLine("UdpClient error. {0}", ex);
				}
			});

			Task.Run(() => {
				try {
					for (;;)
					{
						string port = "";
						lock (this) {

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
										_s = new SerialPort(port);
										//_s.BaudRate = 19200;
										//_s.BaudRate = 115200;
										_s.BaudRate = 38400;
										_s.DataBits = 8;
										_s.Parity = Parity.None;
										_s.StopBits = StopBits.One;
										_s.Handshake = Handshake.None;
										_s.ReadTimeout = 100;
										_s.Open();

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

						var cc = Console.ForegroundColor;
						Console.ForegroundColor = ConsoleColor.Red;
						Console.Error.WriteLine("===> Reading from {0} port <====", port);
						Console.ForegroundColor = cc;

						while (this.Cmd == 'R') {
							try {
								int b = _s.ReadByte();
								char ch = (char)b;
								Console.Write("{0}", ch);
							} catch (TimeoutException) {
							}
						}

						if (this.Cmd == 'S')
						{
							Console.ForegroundColor = ConsoleColor.Red;
							Console.Error.WriteLine("===> Releasing COM port <====");
							Console.ForegroundColor = cc;

							lock(this) {
								_s.Close();
								_s = null;
								this.Cmd = '?';
							}
						}
						while (this.Cmd != 'R')
						{
							Thread.Sleep(100);
						}
					}
				}
				catch (Exception ex)
				{
					Console.WriteLine("Task.Run error. {0}", ex);
				}
			});


			try {
				for (;;)
				{
					var r = Console.ReadKey();
					string ss = r.KeyChar.ToString();

					lock (this) {
						if (_s != null)
							_s.Write(ss);
					}
				}
			}
			catch (Exception ex) {
				Console.WriteLine("ReadKey error. {0}", ex);
			}

		}

		static void sendCmd(char cmd)
		{
			using (var c = new UdpClient()) {
				byte[] b = new byte[1];
				b[0] = (byte)cmd;
				c.Send(b, 1, "localhost", cPort);
				Thread.Sleep(500);
			}
		}

		public static void Main(string[] args) {
			if (args.Length == 1 && args[0] == "-stop")
				sendCmd('S');
			else if (args.Length == 1 && args[0] == "-run")
				sendCmd('R');
			else
				new MainClass().begin();
		}
	}
}
