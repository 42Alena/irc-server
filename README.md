# ft_irc — Poll-based IRC Server (C++98)

A lightweight Internet Relay Chat (IRC) server written in **C++98**.  
Single-process, **non-blocking I/O**, and a **single `poll()` loop** handle all clients concurrently.  
Works with standard IRC clients (e.g., Weechat, HexChat, irssi).

---

##  What it does

- **Listens on a TCP port** (IPv4/IPv6) and accepts multiple clients at once  
- **Non-blocking** reads/writes; **one `poll()`** manages read, write, and listen events  
- **Authenticates** users and lets them set **nicknames** / **usernames**  
- **Private messages** (user ↔ user) and **channel messages** (broadcast to members)  
- **Channels** with operators, topics, keys, invite-only, and user limits  
- **Operator tools:** `KICK`, `INVITE`, `TOPIC`, `MODE` (`i t k o l` flags)

---

##  Build & Run

```bash
# Build (example)
make

# Run
./ircserv <port> <password>
# e.g.
./ircserv 6667 secretpass
```
## Connect with your favorite IRC client:
```bash
# irssi
irssi -c localhost -p 6667
# then inside irssi:
# /nick Alice
# /join #42
``
