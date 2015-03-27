import httplib
conn = httplib.HTTPConnection("www.myip.net")
conn.request("GET", "/")
r1 = conn.getresponse()
print r1.read()
r1.close()
