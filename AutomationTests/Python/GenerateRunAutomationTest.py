# Created By Tiger Shan, according to AutomationTestData.csv
# this will auto create different .bat files for different unit testing cases.
# the .bat files will then be called through by Github actions. CI pipeline
# after all .bat files done with execution, there will be a report in .json format
# coding: utf-8

import unreal
import csv
import os

# csvdir =os.path.dirname(__file__)
# csvfilename = os.path.join(csvdir, 'AutomationTestsData.csv')

batdir = os.path.dirname(os.path.dirname(__file__))
batfilename = batdir+'/Setup/RunAutomationTest.bat'

csvfilename = batdir+'/Data/AutomationTestsData.csv'

batfile = open(batfilename)
lines = batfile.readlines()

newbatfilenames = []
newbatfilename = ''

with open(csvfilename) as csv_file:
    csv_reader = csv.reader(csv_file, delimiter=',')
    line_count = 0
    
    for row in csv_reader:
        if line_count == 0:
            headers = row
            line_count += 1
        else:
            new_text = []
            newbatfilename = batdir+'/RunTest_'+row[0]+'.bat'
            newbatfilenames.append(newbatfilename)
            for line in lines:
                haveModified=0
                for h in headers:
                    if h+'=' in line:
                        new_text.append(line.replace(line,'set '+h+'='+row[headers.index(h)])+'\n')                   
                        haveModified=1
                        break                    
                    if h=='ClientType':
                        str_type = row[headers.index(h)]
                        str_split = str_type.split('#')                        
                        if 'AutomationTest_ClientPartner.bat' in line:
                            for i in range(0,len(str_split)-1):
                                str_to_append = 'start "%run_bat_title%" AutomationTest_ClientPartner.bat'+' %1='+ str_split[i]+'\n'
                                if i != len(str_split)-2:
                                    str_to_append += 'ping 127.0.0.1 -n 3'+'\n'
                                new_text.append(str_to_append)
                            haveModified=1
                            break
                        if 'AutomationTest_ClientTest.bat' in line:
                            str_to_append = 'start "%run_bat_title%" AutomationTest_ClientTest.bat %1=%MapName%.%TestName%' + ' %2=' + str_split[len(str_split)-1]+'\n'
                            new_text.append(str_to_append)  
                            haveModified=1
                            break                          
                if haveModified==0:
                    new_text.append(line)    
            line_count += 1

        isFolderExist=os.path.exists(batdir) 
        if not isFolderExist:
            os.makedirs(batdir)

        if newbatfilename!='':
            with open(newbatfilename,'w') as f:
                for text in new_text:
                    f.write(text)
                print(new_text)
        
    print(f'Processed {line_count} lines.')



# for s in lines:
#     print(s)

batfile.close()

