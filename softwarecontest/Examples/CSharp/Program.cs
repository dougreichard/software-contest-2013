using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net.Sockets;
using System.IO;
using Newtonsoft.Json.Linq;


namespace CSharp
{
    class GamePlayer
    {
        TcpClient client;


        public delegate string GameStartDelegate(string id);
        public event GameStartDelegate GameStart;

        public delegate string PlayerNameDelegate();
        public event PlayerNameDelegate PlayerName;

        public delegate string GameTurnDelegate(string card);
        public event GameTurnDelegate GameTurn;

        public delegate string GameStateDelegate(string card);
        public event GameStateDelegate GameState;


        public void Start()
        {
            int readsize = 1024;

            client = new TcpClient("127.0.0.1", 8124);
            Stream stream = client.GetStream();
            UTF8Encoding encoder = new UTF8Encoding();
            Byte[] data = new Byte[readsize];

            string name = PlayerName();
            // Write Name Command
            
            data = encoder.GetBytes(name);
            stream.Write(data, 0, data.Length);

            bool done = false;
            while (!done)
            {
                // Receive Game Start
                data = new Byte[readsize];
                stream.Read(data, 0, data.Length);
                string json = encoder.GetString(data);

                if (json.Contains("\"id\":"))
                {
                    string resp = GameStart(json);
                    // Write Ready
                    data = encoder.GetBytes(resp);
                    stream.Write(data, 0, data.Length);
                }
                else if (json.Contains("\"card\":"))
                {
                    // Parse Json
                    string push = GameTurn(json);
                    // winner != zero ingame = false;
                    // Write Push commands
                    data = encoder.GetBytes(push);
                    stream.Write(data, 0, data.Length);

                }
                else if (json.Contains("\"winner\":"))
                {
                    string push = GameState(json);
                    // winner != zero ingame = false;
                    // Write Push commands
                    data = encoder.GetBytes(push);
                    stream.Write(data, 0, data.Length);
                }


            }

        }

    }
    
    
    
    class Program
    {
        static void Main(string[] args)
        {
            GamePlayer player = new GamePlayer();
            player.GameStart += new GamePlayer.GameStartDelegate(player_GameStart);
            player.GameTurn += new GamePlayer.GameTurnDelegate(player_GameTurn);
            player.PlayerName += new GamePlayer.PlayerNameDelegate(player_PlayerName);
            player.GameState += new GamePlayer.GameStateDelegate(player_GameState);
            player.Start();

        }

        static string player_GameState(string state)
        {
            // Handle receiving game state
            JObject game = JObject.Parse(state);
            
            // 0= no winner, 1=playerOne 2=Player2
            // Game start has your player number
            int winner = game["winner"].ToObject<int>(); 


            // Return ready
            JObject n = new JObject();
            n.Add("ready", true);
            return n.ToString();
            
        }

        static string player_PlayerName()
        {
            System.Console.WriteLine("Said my name");
            JObject n = new JObject();
            n.Add("name", "fred");

            return n.ToString();
        }

        static string player_GameTurn(string card)
        {
            // Parses the message data to be used in C#
            JObject data = JObject.Parse(card);

            System.Console.WriteLine(data.ToString());

            System.Console.WriteLine("Here is the card parsed");
            //
            for (int row = 0; row < 5; row++)
            {
                for (int col = 0; col < 5; col++)
                {
                    System.Console.Write(data["card"][row][col]);
                }
                System.Console.WriteLine();
            }


            System.Console.WriteLine("Took a turn");
            

            System.Console.WriteLine("Left looks good");

            JObject ret = new JObject();
            ret.Add("push", "L");

            return ret.ToString();
        }

        static string player_GameStart(string id)
        {
            
            // This is the server ID of your opponent not name
            System.Console.WriteLine("Game Starting");
            System.Console.WriteLine(id);

            JObject data = JObject.Parse(id);
            int otherPlayerID = data["id"].ToObject<int>();
            System.Console.WriteLine("Other player {0}",otherPlayerID);
            int playerNum = data["player"].ToObject<int>();
            System.Console.WriteLine("In this game I am Player {0}", playerNum);

            // Return ready
            JObject n = new JObject();
            n.Add("ready", true);
            return n.ToString();


        }
    }
}
