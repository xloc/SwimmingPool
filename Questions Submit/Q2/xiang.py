with open('text.txt') as f: 
	string_list=f.read().lower().split()
mat=[]
count=[]
for word in string_list:
	if word not in mat:
		mat.append(word)
	 	count.append(string_list.count(word))
count.sort()
print count[len(count)-9:len(count)]