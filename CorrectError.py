import os 

for i in range(0, 20):
    n = []
    s = []
    print 'Batch: ' + str(i)
    string = raw_input('Enter in the format <error> <replacement> ... ')
    if string == '':
        continue
    command = string.split(' ')
    for j in range(0, len(command)):
        if j % 2 == 0:
            n.append(command[j])
        else:
            s.append(command[j])        
    for k in range(0, len(n)):
        print(str(n[k]), str(s[k]))
        os.remove('ResultantStorage/' + str(i) + '/' + str(n[k]) + '.png')
        print(str(n[k]), str(s[k]))
        os.rename('ResultantStorage/' + str(i) + '/' + str(s[k]) + '.png', 'ResultantStorage/' + str(i) + '/' + str(n[k]) + '.png')
    