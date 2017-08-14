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

		void begin(string tty) {

			try {
				_s = new SerialPort("/dev/ttyUSB0");
				//_s.BaudRate = 19200;
				_s.BaudRate = 115200;
				//_s.BaudRate = 38400;
				_s.DataBits = 8;
				_s.Parity = Parity.None;
				_s.StopBits = StopBits.One;
				_s.Handshake = Handshake.None;
				_s.ReadTimeout = 100;
				_s.Open();

				byte [] v = { 0x55, 0x55, 0x00, 0x00, 0x00};
				_s.Write(v, 0, v.Length);

				//while (_s.ByteToRead>0) _s.ReadByte();

				byte []v1 = {0x00,0x00,0xFF,0x02,0xFE,0xD4,0x02,0x2A,0x00};
				_s.Write(v1, 0, v1.Length);

				for (;;)
				{
					int r =_s.ReadByte();
					if (r < 0) continue;

					Console.WriteLine((byte)r);
				}

			}
			catch (Exception ex)
			{
				Console.WriteLine("Task.Run error. {0}", ex);
			}
		}


		public static void Main(string[] args) {
			new MainClass().begin(null);
		}
	}
}
