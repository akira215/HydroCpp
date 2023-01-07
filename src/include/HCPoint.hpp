/*
  HydroCpp
  Repository: https://github.com/btzy/nativefiledialog-extended
  License: GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
  Author: Akira Shimahara
*/


namespace HydroCpp
{
    class HCPoint
    {
    public:
        /**
         * @brief constructor
         * @param x
         * @param y
         */
        HCPoint(double x, double y);

        /**
         * @brief
         */
        ~HCPoint();
        
        /**
         * @brief Copy constructor
         * @param other The object to be copied.
         */
        HCPoint(const HCPoint& other);

        /**
         * @brief Move constructor
         * @param other The XLCell object to be moved
         */
        HCPoint(HCPoint&& other);

        /**
         * @brief Copy assignment operator
         * @param other The object to be copy assigned
         * @return A reference to the new object
         */
        HCPoint& operator=(const HCPoint& other);

        /**
         * @brief Move assignment operator [deleted]
         * @param other The object to be move assigned
         * @return A reference to the new object
         */
        HCPoint& operator=(HCPoint&& other);

        /**
         * @brief Add operator
         * @param rhs The object to be copy assigned
         * @return A reference to the object
         */
        HCPoint& operator+(const HCPoint& rhs);

        /**
         * @brief Sub operator
         * @param other The object to be copy assigned
         * @return A reference to the object
         */
        HCPoint& operator-(const HCPoint& rhs);

        /**
         * @brief Equality operator
         * @param other The object to be copy assigned
         * @return A reference to the object
         */
        bool operator==(const HCPoint& other) const;

        /**
         * @brief Inequality operator
         * @param other The object to be copy assigned
         * @return A reference to the object
         */
        bool operator!=(const HCPoint& other) const;

        /**
         * @brief Dot product operator
         * @param other The object to be copy assigned
         * @return the cross product
         */
        double operator*(const HCPoint& rhs) const;

        /**
         * @brief multiply by a double
         * @param m The double to multiply the vect
         * @return A reference to the object
         */
        HCPoint& operator*(const double m);

        /**
         * @brief distance of the point to 0,0
         * @return the distance
         */
        double distanceToOrigin() const;

        /**
         * @brief distance between this an the other point
         * @param other Theother point
         * @return the distance
         */
        double distanceTo(const HCPoint& rhs) const;


    public:
        double x;
        double y;
    };

}  // namespace std