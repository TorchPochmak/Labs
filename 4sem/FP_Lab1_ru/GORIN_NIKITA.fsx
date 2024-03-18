//Горин Никита, М8О-213Б-22
//Вариант 7

//Задание 1. Ряд 7
//-----------------------------------------------------------------------------------
let f x = x * (3. - x) / ((1. - x) * (1. - x) * (1. - x))

let eps = 1e-10 // c System.Double.Epsilon работает долго
let a = 0.0
let b = 0.5
let n = 10

let rec pow_float basee power =
    if power = 0 then 1.
    elif power > 0 then basee * pow_float basee (power - 1)
    else 1. / (pow_float basee -power)

let t_term n x =
    float n * float (n + 2) * pow_float x n

let t_multiplier n x = 
    let fl = float (n - 1)
    (x * fl * fl + 4. * x * fl + 3. * x) / (fl * fl + 2. * fl)

let smart_t x eps =
    let rec rec_calc n prev sum =
        let term = if (n = 1) then (t_term 1 x) else (prev * t_multiplier n x)
        let cumsum = sum + term
        if (abs(term) <= eps) then
            (cumsum, n)
        else
            rec_calc (n + 1) term cumsum
    rec_calc 1 0. 0.

let dumb_t x eps =
    let rec rec_calc n prevSum =
        let term = t_term n x
        let cumsum = prevSum + term
        if (abs(term) <= eps) then
            (cumsum, n)
        else
            rec_calc (n + 1) cumsum
    rec_calc 1 0.

//Печать таблицы
printfn "x\t|\tBuiltin\t\t|\tSmart Taylor\t|\tterms\t|\tDumb Taylor\t|\tterms"
for i=0 to n do
    let x = a+(float i)/(float n)*(b-a)
    let (smart_val, smart_terms) = smart_t x eps
    let (dumb_val, slow_terms) = dumb_t x eps 
    printfn "%5.2f\t|\t%10.6f\t|\t%10.6f\t|\t%d\t|\t%10.6f\t|\t%d" x (f x) (smart_val) (smart_terms) (dumb_val) (slow_terms)

//Задание 2
//Уравнения 7,8,9
//-----------------------------------------------------------------------------------

//Дихотомия
let dichotomy f a b eps =
    let rec rec_calc a b =
        let c = (a + b) / 2.0
        if abs (b - a) <= eps then
            c
        else if f a * f c <= 0.0 then
            rec_calc a c
        else
            rec_calc c b
    rec_calc a b

//Итерации
let rec iterations f x eps =
    let x_new = f x
    if abs (x_new - x) <= eps then x_new
    else iterations f x_new eps

let f_iter f phi x = x - (f x) / (phi x)

let newthon f f' x eps =
    iterations(f_iter f f') x eps


// Уравнение 1: 3 * (ln(x))^2 + 6 * ln(x) - 5 = 0, интервал [1,3]
let f1 x = 3.0 * (log x) ** 2.0 + 6.0 * log x - 5.0
let f'1 x = (6.0 / x) + (6.0 * log x) / x
let phi1 x = 10.

// Уравнение 2: 0.6 * 3^x - 2.3x - 3 = 0, интервал [2,3]
let f2 x = 0.6 * 3.0 ** x - 2.3 * x - 3.0
let f'2 x = 0.6 * (log 3.0) * 3.0 ** x - 2.3
let phi2 x = 10.

// Уравнение 3: x^2 - ln(1 + x) - 3 = 0, интервал [2,3]
let f3 x = x ** 2.0 - log (1.0 + x) - 3.0
let f'3 x = 2.0 * x - 1.0 / (1.0 + x)
let phi3 x = 10.

printfn "%10.5f  %10.5f  %10.5f" (dichotomy f1 1.0 3.0 eps) (iterations (f_iter f1 phi1) 1.5 eps) (newthon f1 f'1 1.5 eps)
printfn "%10.5f  %10.5f  %10.5f" (dichotomy f2 2.0 3.0 eps) (iterations (f_iter f2 phi2) 2.5 eps) (newthon f2 f'2 2.5 eps)
printfn "%10.5f  %10.5f  %10.5f" (dichotomy f3 2.0 3.0 eps) (iterations (f_iter f3 phi3) 2.5 eps) (newthon f3 f'3 2.5 eps)
