using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using MySqlConnector;

namespace XeLive.ClientHandler {
    class MySQL {
        public static MySqlConnection Setup()
        {
            return new MySqlConnection(String.Format("Server={0};Port=3306;Database={1};Uid={2};Password={3};", "45.79.27.181", "XeLive", "XeLive", "XKE0188?!"));
        }

        public static bool Connect(MySqlConnection connection)
        {
            try
            {
                connection.Open();
                return true;
            }
            catch (MySqlException exception)
            {
                Console.WriteLine(exception.Message);
                return false;
            }
        }

        public static void Disconnect(MySqlConnection connection)
        {
            try
            {
                connection.Close();
            }
            catch (MySqlException exception)
            {
                Console.WriteLine(exception.Message);
            }
        }

        public static bool GetClientData(string console_key, ref ClientInfo data)
        {
            using (var db = Setup())
            {
                Connect(db);
                using (var command = db.CreateCommand())
                {
                    command.CommandText = string.Format("SELECT * FROM users WHERE console_key = @key");
                    command.Parameters.AddWithValue("@key", console_key);
                    using (var reader = command.ExecuteReader())
                    {
                        if (reader.Read())
                        {
                            data.iID = reader.GetInt32("id");
                            data.ConsoleKey = reader.GetString("console_key");
                            data.CPUKey = reader.GetString("cpu");
                            data.FirstGamertag = reader.GetString("first_gamertag");
                            data.LastGamertag = reader.GetString("last_gamertag");
                            data.FirstIP = reader.GetString("first_ip");
                            data.LastIP = reader.GetString("last_ip");
                            data.Status = (ClientInfoStatus)reader.GetInt32("status");
                            data.FirstKVHash = reader.GetString("first_kv_hash");
                            data.LastKVHash = reader.GetString("last_kv_hash");
                            data.iLastConnection = Convert.ToInt32(reader.GetString("last_connection"));
                            data.iTotalChallenges = reader.GetInt32("total_challenges");
                            data.iLastUsedVersion = reader.GetInt32("last_version");

                            Disconnect(db);
                            return true;
                        }
                    }
                }
                Disconnect(db);
            }
            return false;
        }

        public static bool GetClientDataFromID(int ID, ref ClientInfo data)
        {
            using (var db = Setup())
            {
                Connect(db);
                using (var command = db.CreateCommand())
                {
                    command.CommandText = string.Format("SELECT * FROM users WHERE `id` = @key");
                    command.Parameters.AddWithValue("@key", ID);
                    using (var reader = command.ExecuteReader())
                    {
                        if (reader.Read())
                        {
                            data.iID = reader.GetInt32("id");
                            data.ConsoleKey = reader.GetString("console_key");
                            data.CPUKey = reader.GetString("cpu");
                            data.FirstGamertag = reader.GetString("first_gamertag");
                            data.LastGamertag = reader.GetString("last_gamertag");
                            data.FirstIP = reader.GetString("first_ip");
                            data.LastIP = reader.GetString("last_ip");
                            data.Status = (ClientInfoStatus)reader.GetInt32("status");
                            data.FirstKVHash = reader.GetString("first_kv_hash");
                            data.LastKVHash = reader.GetString("last_kv_hash");
                            data.iLastConnection = Convert.ToInt32(reader.GetString("last_connection"));
                            data.iTotalChallenges = reader.GetInt32("total_challenges");
                            data.iLastUsedVersion = reader.GetInt32("last_version");

                            Disconnect(db);
                            return true;
                        }
                    }
                }
                Disconnect(db);
            }
            return false;
        }

        public static bool GetKVStats(string kvhash, ref KVStats stats)
        {
            using (var db = Setup())
            {
                Connect(db);
                using (var command = db.CreateCommand())
                {
                    command.CommandText = string.Format("SELECT * FROM kv_stats WHERE kv_hash = @kv_hash");
                    command.Parameters.AddWithValue("@kv_hash", kvhash);
                    using (var reader = command.ExecuteReader())
                    {
                        if (reader.Read())
                        {
                            stats.iID = reader.GetInt32("id");
                            stats.KVHash = reader.GetString("kv_hash");
                            stats.iFirstConnection = Convert.ToInt32(reader.GetString("first_connection"));
                            stats.iLastConnection = Convert.ToInt32(reader.GetString("last_connection"));
                            stats.bBanned = reader.GetBoolean("banned");
                            stats.iBannedTime = Convert.ToInt32(reader.GetString("banned_time"));
                            stats.iTotalChallenges = reader.GetInt32("total_challenges");

                            Disconnect(db);
                            return true;
                        }
                    }
                }
                Disconnect(db);
            }
            return false;
        }

        public static void UpdateKVStat(string kvhash, bool banned)
        {
            KVStats cur = new KVStats();
            GetKVStats(kvhash, ref cur);

            using (var db = Setup())
            {
                Connect(db);
                using (var command = db.CreateCommand())
                {
                    command.CommandText = string.Format("UPDATE kv_stats SET last_connection = @last_connection, banned = @banned, banned_time = @banned_time WHERE `kv_hash` = @kv_hash");
                    command.Parameters.AddWithValue("@last_connection", banned ? cur.iLastConnection : Utils.GetTimeStamp());
                    command.Parameters.AddWithValue("@banned", banned);
                    command.Parameters.AddWithValue("@banned_time", banned && !cur.bBanned ? (int)Utils.GetTimeStamp() : cur.iBannedTime);
                    command.Parameters.AddWithValue("@kv_hash", kvhash);
                    command.ExecuteNonQuery();
                }
                Disconnect(db);
            }
        }

        public static void AddKVStat(string kvhash, int firstConnection, int lastConnection, bool banned, int bannedTime)
        {
            using (var db = Setup())
            {
                Connect(db);
                using (var command = db.CreateCommand())
                {
                    command.CommandText = string.Format("INSERT INTO kv_stats (kv_hash, first_connection, last_connection, banned, banned_time) VALUES (@kv_hash, @first_connection, @last_connection, @banned, @banned_time)");
                    command.Parameters.AddWithValue("@kv_hash", kvhash);
                    command.Parameters.AddWithValue("@first_connection", firstConnection);
                    command.Parameters.AddWithValue("@last_connection", lastConnection);
                    command.Parameters.AddWithValue("@banned", banned);
                    command.Parameters.AddWithValue("@banned_time", bannedTime);
                    command.ExecuteNonQuery();
                }
                Disconnect(db);
            }
        }

        public static List<XexInfo> GetXexInfos()
        {
            List<XexInfo> xexInfos = new List<XexInfo>();

            using (var db = Setup())
            {
                Connect(db);
                using (var command = db.CreateCommand())
                {
                    command.CommandText = string.Format("SELECT * FROM xex_data");
                    using (var reader = command.ExecuteReader())
                    {
                        if (reader.HasRows)
                        {
                            while (reader.Read())
                            {
                                XexInfo data = new XexInfo
                                {
                                    iID = reader.GetInt32("id"),
                                    dwLastVersion = reader.GetUInt32("latest_version"),
                                    Name = reader.GetString("name"),
                                    dwTitle = Convert.ToUInt32(reader.GetString("title")),
                                    dwTitleTimestamp = Convert.ToUInt32(reader.GetString("title_timestamp")),
                                    bEnabled = reader.GetBoolean("enabled"),
                                };

                                xexInfos.Add(data);
                            }
                        }
                    }
                }
                Disconnect(db);
            }

            return xexInfos;
        }

        public static bool GetXexInfo(int id, ref XexInfo data)
        {
            using (var db = Setup())
            {
                Connect(db);
                using (var command = db.CreateCommand())
                {
                    command.CommandText = string.Format("SELECT * FROM xex_data WHERE `id` = @key");
                    command.Parameters.AddWithValue("@key", id);
                    using (var reader = command.ExecuteReader())
                    {
                        if (reader.Read())
                        {
                            data.iID = reader.GetInt32("id");
                            data.dwLastVersion = reader.GetUInt32("latest_version");
                            data.Name = reader.GetString("name");
                            data.dwTitle = Convert.ToUInt32(reader.GetString("title"));
                            data.dwTitleTimestamp = Convert.ToUInt32(reader.GetString("title_timestamp"));
                            data.bEnabled = reader.GetBoolean("enabled");
                            Disconnect(db);
                            return true;
                        }
                    }
                }
                Disconnect(db);
            }
            return false;
        }

        public static void UpdateUserInfoWelcomePacket(string consolekey, string kvhash, string ip)
        {
            using (var db = Setup())
            {
                Connect(db);
                using (var command = db.CreateCommand())
                {
                    command.CommandText = string.Format("UPDATE users SET last_kv_hash = @last_kv_hash, last_ip = @last_ip, last_connection = @last_connection WHERE `console_key` = @console_key");
                    command.Parameters.AddWithValue("@last_kv_hash", kvhash);
                    command.Parameters.AddWithValue("@last_ip", ip);
                    command.Parameters.AddWithValue("@console_key", consolekey);
                    command.Parameters.AddWithValue("@last_connection", Utils.GetTimeStamp());
                    command.ExecuteNonQuery();
                }
                Disconnect(db);
            }
        }

        public static void AddUserWelcomePacket(string consolekey, string cpu, string ip, string kvhash)
        {
            using (var db = Setup())
            {
                Connect(db);
                using (var command = db.CreateCommand())
                {
                    command.CommandText = string.Format("INSERT INTO users (console_key, cpu, first_ip, last_ip, status, first_kv_hash, last_kv_hash) VALUES (@console_key, @cpu, @first_ip, @last_ip, @status, @first_kv_hash, @last_kv_hash)");

                    command.Parameters.AddWithValue("@console_key", consolekey);
                    command.Parameters.AddWithValue("@cpu", cpu);
                    command.Parameters.AddWithValue("@first_ip", ip);
                    command.Parameters.AddWithValue("@last_ip", ip);
                    command.Parameters.AddWithValue("@status", ClientInfoStatus.Authed);
                    command.Parameters.AddWithValue("@first_kv_hash", kvhash);
                    command.Parameters.AddWithValue("@last_kv_hash", kvhash);

                    command.ExecuteNonQuery();
                }
                Disconnect(db);
            }
        }

        public static void IncrementChallengeCount(string console_key)
        {
            string lastkvhash = "";
            using (var db = Setup())
            {
                Connect(db);
                using (var command = db.CreateCommand())
                {
                    command.CommandText = string.Format("SELECT * FROM users WHERE `console_key` = @console_key");
                    command.Parameters.AddWithValue("@console_key", console_key);
                    using (var reader = command.ExecuteReader())
                    {
                        if (reader.Read())
                        {
                            lastkvhash = reader.GetString("last_kv_hash");
                        }
                    }
                }
                Disconnect(db);
            }

            using (var db = Setup())
            {
                Connect(db);
                using (var command = db.CreateCommand())
                {
                    command.CommandText = string.Format("UPDATE users SET total_challenges=total_challenges+1 WHERE `console_key` = @console_key");
                    command.Parameters.AddWithValue("@console_key", console_key);
                    command.ExecuteNonQuery();
                }
                Disconnect(db);
            }

            if (lastkvhash.Length > 1)
            {
                using (var db = Setup())
                {
                    Connect(db);
                    using (var command = db.CreateCommand())
                    {
                        command.CommandText = string.Format("UPDATE kv_stats SET total_challenges=total_challenges+1 WHERE `kv_hash` = @kv_hash");
                        command.Parameters.AddWithValue("@kv_hash", lastkvhash);
                        command.ExecuteNonQuery();
                    }
                    Disconnect(db);
                }
            }
        }

        public static void UpdateUserGamertag(ClientInfo info, string gamertag)
        {
            using (var db = Setup())
            {
                Connect(db);
                using (var command = db.CreateCommand())
                {
                    if (info.FirstGamertag == "--blankuser--")
                    {
                        command.CommandText = string.Format("UPDATE users SET first_gamertag = @first, last_gamertag = @last_gamertag WHERE `console_key` = @console_key");
                        command.Parameters.AddWithValue("@first", gamertag);
                    }
                    else
                    {
                        command.CommandText = string.Format("UPDATE users SET last_gamertag = @last_gamertag WHERE `console_key` = @console_key");
                    }

                    command.Parameters.AddWithValue("@last_gamertag", gamertag);
                    command.Parameters.AddWithValue("@console_key", info.ConsoleKey);
                    command.ExecuteNonQuery();
                }
                Disconnect(db);
            }
        }

        public static void AddRequestToken(string token, string console_key) {
            using (var db = Setup()) {
                Connect(db);
                using (var command = db.CreateCommand()) {
                    command.CommandText = string.Format("INSERT INTO access_tokens (connection_index, `token`, `console_key`, last_connection) VALUES (@connection_index, @token, @console_key, @last)");
                    command.Parameters.AddWithValue("@connection_index", 1);
                    command.Parameters.AddWithValue("@token", token);
                    command.Parameters.AddWithValue("@console_key", console_key);
                    command.Parameters.AddWithValue("@last", Utils.GetTimeStamp());
                    command.ExecuteNonQuery();
                }
                Disconnect(db);
            }
        }
    }
}
