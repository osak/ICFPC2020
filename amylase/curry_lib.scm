; constants
(define t (lambda (x) (lambda (y) x)))
(define f (lambda (x) (lambda (y) y)))

(define nil (lambda (x) t))


; util
(define mybool (lambda (b) (if b t f)))


; arithmetics
(define inc (lambda (x) (+ (force x) 1)))
(define add (lambda (x) (lambda (y) (+ (force x) (force y)))))
(define mul (lambda (x) (lambda (y) (* (force x) (force y)))))

(define mydiv (lambda (x) (lambda (y) 
    (let ((fx (force x)) (fy (force y))) 
        (if (fx >= 0) (div fx fy) (div -fx -fy))
    )
)))

(define eq (lambda (x) (lambda (y) (mybool (= (force x) (force y))))))
(define lt (lambda (x) (lambda (y) (mybool (< (force x) (force y))))))

(define neg (lambda (x) (* (force x) -1)))


; combinators
(define s (lambda (x) (lambda (y) (lambda (z) 
    (let ((fx (force x)) (fy (force y)) (fz (force z))) 
        ((x z) (y z))
    )
))))
(define c (lambda (x) (lambda (y) (lambda (z) 
    (let ((fx (force x)) (fy (force y)) (fz (force z)))
        ((x z) y)
    )
))))
(define b (lambda (x) (lambda (y) (lambda (z) 
    (let ((fx (force x)) (fy (force y)) (fz (force z)))
        (x (y z))
    )
))))
(define i (lambda (x) x))


; structures
(define mycons (lambda (x) (lambda (y) (lambda (z) ((z x) y)))))
(define mycar (lambda (x) ((force x) t)))
(define mycdr (lambda (x) ((force x) f)))

(define isnil (lambda (x) (if (eq? (force x) nil) t f)))


; result
(define calc (lambda (gal) 
    (((force gal) nil) ((mycons 0) 0))
))

