import sympy 
import xlrd

def read_sales():
    workbook = xlrd.open_workbook(r'C:\Users\吴天元\Documents\Tencent Files\898368098\FileRecv\work.xlsx')
    return workbook

class Sale_and_influence:

    def __init__(self,workbook):
        self.workbook = workbook

    def get_highT(self,workbook):
        sheet_names= workbook.sheet_names()
        for sheet_name in sheet_names:
            sheet2 = workbook.sheet_by_name(sheet_name)
            highT = sheet2.col_values(11) 
        return highT

    def get_lowT(self,workbook):
        sheet_names= workbook.sheet_names()
        for sheet_name in sheet_names:
            sheet2 = workbook.sheet_by_name(sheet_name)
            lowT = sheet2.col_values(12) 
        return lowT

    def get_wind(self,workbook):
        sheet_names= workbook.sheet_names()
        for sheet_name in sheet_names:
            sheet2 = workbook.sheet_by_name(sheet_name)
            chips_sales = sheet2.col_values(15) 
        return chips_sales

    def get_air_quality(self,workbook):
        sheet_names= workbook.sheet_names()
        for sheet_name in sheet_names:
            sheet2 = workbook.sheet_by_name(sheet_name)
            air_quality = sheet2.col_values(17) 
        return air_quality

    def get_sales(self,workbook):
        sheet_names= workbook.sheet_names()
        for sheet_name in sheet_names:
            sheet2 = workbook.sheet_by_name(sheet_name)
            sales = sheet2.col_values(2) 
        return sales

def price_function(workbook):
    k1,k2,k3,k4,k5 = sympy.symbols('k1 k2 k3 k4 k5')
    datas = Sale_and_influence(workbook)
    highT = datas.get_highT(workbook)
    lowT = datas.get_lowT(workbook)
    air_quality = datas.get_air_quality(workbook)
    wind = datas.get_wind(workbook)
    prices = datas.get_sales(workbook)
    price_error = 0
    for i in range(len(wind)):
        p_price = k1 * highT[i] + k2 * lowT[i] + k3 * air_quality[i] + k4 * wind[i] + k5
        per_error = (p_price - prices[i])**2
        price_error += per_error
    return price_error

def grad(function,a1,a2,a3,a4,a5):
    k1,k2,k3,k4,k5 = sympy.symbols('k1 k2 k3 k4 k5')
    fk1, fk2, fk3, fk4, fk5 = sympy.diff(function,k1), sympy.diff(function,k2), sympy.diff(function,k3), sympy.diff(function,k4), sympy.diff(function,k5)
    fk1 = fk1.subs({k1:a1, k2:a2, k3:a3, k4:a4, k5:a5})
    fk2 = fk2.subs({k1:a1, k2:a2, k3:a3, k4:a4, k5:a5})
    fk3 = fk3.subs({k1:a1, k2:a2, k3:a3, k4:a4, k5:a5})
    fk4 = fk4.subs({k1:a1, k2:a2, k3:a3, k4:a4, k5:a5})
    fk5 = fk5.subs({k1:a1, k2:a2, k3:a3, k4:a4, k5:a5})
    return [fk1,fk2,fk3,fk4,fk5]

def main():
    k1,k2,k3,k4,k5 = sympy.symbols('k1 k2 k3 k4 k5')
    workbook = read_sales()
    a1, a2, a3, a4, a5 = 0, 0, 0, 0, 42
    price_error = price_function(workbook)
    price_err = price_error.subs({k1:a1, k2:a2, k3:a3, k4:a4, k5:a5})
    while (price_err > 400):
        price_error = price_function(workbook) #a function
        price_err = price_error.subs({k1:a1, k2:a2, k3:a3, k4:a4, k5:a5})
        gradf = grad(price_error, a1, a2, a3, a4, a5)
        dis = ((gradf[0])**2 + (gradf[1])**2 + (gradf[2])**2 + (gradf[3])**2 + (gradf[4])**2)**0.5
        lam = 0.01
        da1, da2, da3, da4, da5 = lam*gradf[0]/dis, lam*gradf[1]/dis, lam*gradf[2]/dis, lam*gradf[3]/dis, lam*gradf[4]/dis
        a1 = a1 - da1
        a2 = a2 - da2
        a3 = a3 - da3
        a4 = a4 - da4
        a5 = a5 - da5       
    print (a1, a2, a3, a4, a5)

if __name__ == "__main__":
    main()



    

    









