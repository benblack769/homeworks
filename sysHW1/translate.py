fname = "graphdata4.csv"

f = open(fname)
lines = f.readlines()
f.close()

fnamesnew = ["graphdata"+str(n)+".col" for n in range(5)]
for n,fnamenew in enumerate(fnamesnew):
	data = eval("["+lines[n]+"]")
	wf = open(fnamenew,"w")
	for i,d in enumerate(data):
		wf.write(str(2**(i+8)) +"\t\t"+ str(d) + "\n")
	wf.close()
