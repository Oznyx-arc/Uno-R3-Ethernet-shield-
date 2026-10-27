void printIp(const byte *v) {
  Serial.print(v[0]); Serial.print('.'); Serial.print(v[1]);
  Serial.print('.'); Serial.print(v[2]); Serial.print('.'); Serial.print(v[3]);
}
void startEthernet() {
  if (settings.useDhcp) {
    Serial.println(F("Requesting DHCP lease..."));
    if (Ethernet.begin(mac) != 0) {
      Serial.print(F("DHCP address: http://"));
      Serial.println(Ethernet.localIP());
      return;
    }
    Serial.println(F("DHCP failed; using saved static fallback."));
  }
  Ethernet.begin(mac, IPAddress(settings.ip), IPAddress(settings.dns),
                 IPAddress(settings.gateway), IPAddress(settings.subnet));
  Serial.print(F("Static address: http://"));
  Serial.println(Ethernet.localIP());
}
// Gets a decimal IPv4 address from a URL query field such as ip=192.168.1.20.
bool queryIp(const char *line, const char *key, byte out[4]) {
  char findKey[12];
  snprintf(findKey, sizeof(findKey), "%s=", key);
  const char *p = strstr(line, findKey);
  if (!p) return false;
  p += strlen(findKey);
  for (byte i = 0; i < 4; ++i) {
    char *end;
    long n = strtol(p, &end, 10);
    if (end == p || n < 0 || n > 255) return false;
    out[i] = (byte)n;
    if (i < 3) {
      if (*end != '.') return false;
      p = end + 1;
    }
  }
  return true;
}
void sendPage(EthernetClient &client, const __FlashStringHelper *notice) {
  client.println(F("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: close\r\n"));
  client.println(F("<!doctype html><html><head><meta name=viewport content='width=device-width,initial-scale=1'>"
                   "<title>Uno Ethernet setup</title><style>body{font-family:sans-serif;max-width:38rem;margin:2rem auto;padding:0 1rem}"
                   "input{width:15rem;margin:.25rem}label{display:block}button{margin-top:1rem;padding:.5rem}</style></head><body>"));
  client.println(F("<h1>Uno Ethernet setup</h1><p>Save changes, then reset the Uno or cycle its power.</p>"));
  if (notice) { client.print(F("<p><strong>")); client.print(notice); client.println(F("</strong></p>")); }
  client.print(F("<form action='/save'><label><input type=checkbox name=dhcp "));
  if (settings.useDhcp) client.print(F("checked"));
  client.println(F("> Obtain address automatically (DHCP)</label>"));
  client.print(F("<label>Static IP <input name=ip value='")); printIpToClient(client, settings.ip); client.println(F("'></label>"));
  client.print(F("<label>Subnet mask <input name=mask value='")); printIpToClient(client, settings.subnet); client.println(F("'></label>"));
  client.print(F("<label>Gateway <input name=gw value='")); printIpToClient(client, settings.gateway); client.println(F("'></label>"));
  client.print(F("<label>DNS server <input name=dns value='")); printIpToClient(client, settings.dns); client.println(F("'></label><button>Save configuration</button></form>"));
  client.println(F("<p>Current address: "));
  client.print(Ethernet.localIP());
  client.println(F("</p></body></html>"));
}
void printIpToClient(EthernetClient &client, const byte *v) {
  client.print(v[0]); client.print('.'); client.print(v[1]); client.print('.');
  client.print(v[2]); client.print('.'); client.print(v[3]);
}
void handleRequest(char *line, EthernetClient &client) {
  if (strncmp(line, "GET /save?", 10) == 0) {
    NetworkSettings candidate = settings;
    candidate.useDhcp = strstr(line, "dhcp=on") ? 1 : 0;
    bool ok = queryIp(line, "ip", candidate.ip) && queryIp(line, "mask", candidate.subnet) &&
              queryIp(line, "gw", candidate.gateway) && queryIp(line, "dns", candidate.dns);
    if (ok) { settings = candidate; saveSettings(); sendPage(client, F("Saved. Reset the Uno to apply it.")); }
    else sendPage(client, F("Not saved: enter all four valid IPv4 addresses."));
  } else {
    sendPage(client, 0);
  }
}
void setup() {
  Serial.begin(9600);
  if (!loadSettings()) { defaultSettings(); saveSettings(); }
  startEthernet();
  server.begin();
}
void loop() {
  EthernetClient client = server.available();
  if (!client) return;
  char firstLine[180];
  byte pos = 0;
  bool firstLineDone = false;
  unsigned long deadline = millis() + 1500;
  while (client.connected() && millis() < deadline) {
    while (client.available()) {
      char c = client.read();
      if (!firstLineDone && c != '\r' && c != '\n' && pos < sizeof(firstLine) - 1) firstLine[pos++] = c;
      if (c == '\n') {
        if (!firstLineDone) { firstLine[pos] = 0; firstLineDone = true; }
        else { handleRequest(firstLine, client); delay(2); client.stop(); return; }
      }
    }
  }
  client.stop();
}
