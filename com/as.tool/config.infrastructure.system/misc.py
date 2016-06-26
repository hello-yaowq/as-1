import xlrd

def parse_as_xls_com(filename):
    scom = {}
    book = xlrd.open_workbook(filename)
    sheet = book.sheet_by_name('COM')
    for row in range(5,sheet.nrows):
        signal = []
        for col in range(1,sheet.ncols):
            signal.append(str(sheet.cell(row,col).value))
        name = signal[0]
        try:
            scom[name].append(signal)
        except KeyError:
            scom[name]=[signal]
    return scom

if(__name__ == '__main__'):
    for name,signals in parse_as_xls_com('AS.xlsm').items():
        print('%s = {'%(name))
        for sig in signals:
            print('\t%s'%(sig))
        print('}')

