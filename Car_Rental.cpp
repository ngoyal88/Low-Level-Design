#include <bits/stdc++.h>
using namespace std;


/* ========== ENUMS ========== */
enum  CarType {SUV, Sedan};
enum  CarStatus {Available, Reserved, UnderMaintenance};
enum  PaymentMode {UPI, Cash, Card};

/* =======MODELS======*/
struct User {
	string id;
	string name;
	string email;
	string phoneNo;
};
struct Car {
	string carId;
	string carName;
	CarType carType;
	CarStatus carStatus;
};
struct Booking {
	string id;
	User user;
	Car car;
	int fromDay;
	int toDay;
	double amount;
	PaymentMode paymentMode;
	bool isActive;
};

/*====Stratergy====*/
struct PricingStratergy {
	double calculatePrice(Car car, int fromDay, int toDay) {
		int days = toDay - fromDay;
		if (days <= 0) days = 1;
		return 100.00 * days;
	}
};

struct FineStratergy {
	double calculateFine(int actualReturn, int currentDay) {
		int daysLate = currentDay - actualReturn;
		if (daysLate <= 0) return 0.0;
		return 110.0 * daysLate;
	}
};

/*===PAYMENT===*/
struct Payment {
	virtual void pay(double amount) {
		cout << "Paid :" << amount << endl;
	}
};
struct UPIPayment : Payment {
	void pay(double amount) override {
		cout << "Paid :" << amount << " via UPI" << endl;
	}
};
struct CashPayment : Payment {
	void pay(double amount) override {
		cout << "Paid :" << amount << " in Cash" << endl;
	}
};
struct CardPayment : Payment {
	void pay(double amount) override {
		cout << "Paid :" << amount <<" using Card"<<endl;
	}
};

struct PaymentFactory{
	static Payment* getPaymentMethod(PaymentMode mode){
		if(mode == UPI) return new UPIPayment();
		if(mode == Cash) return new CashPayment();
		if(mode == Card) return new CardPayment();
		return NULL;  
	} 
};

/*===Car Repository===*/
struct CarRepository{
	map<string,Car> carsDb;
	void addCar(Car car){
		carsDb[car.carId] = car;
	}

	vector<Car> getAllCars(){
		vector<Car> result;
		for(auto &p : carsDb){
			result.push_back(p.second);
		}
		return result;
	}
	bool isAvailable(string id){
		return carsDb[id].carStatus == Available;
	}

	void reserveCar(string id){
		carsDb[id].carStatus = Reserved;
	}
	void markAvailable(string id){
		carsDb[id].carStatus = Available;
	}
};

/*===Booking Repository===*/
struct BookingRepository{
	map<string,Booking> bookingDb;

	void addBooking(Booking b){
		bookingDb[b.id] = b;
	}
	Booking* getBooking(string id){
		if(bookingDb.find(id) == bookingDb.end()) return NULL;
		return &bookingDb[id];
	}
	void removeBooking(string id){
		bookingDb.erase(id);
	}
};
/*=== Services==*/
struct CarRentalService{
	CarRepository* carRepo;
	BookingRepository* bookingRepo;
	PricingStratergy pricing;
	FineStratergy fine;

	Booking issueCar(User user, Car car,int fromDay,int toDay,PaymentMode mode){
		if(!carRepo->isAvailable(car.carId)){
			cout<<"Car not available"<<endl;
			exit(0);
		}
		carRepo->reserveCar(car.carId);

		double amount = pricing.calculatePrice(car,fromDay,toDay);
		Payment* payment = PaymentFactory::getPaymentMethod(mode);
		payment->pay(amount);
		delete  payment;

		Booking b;
		b.id = "B" + car.carId;
		b.user = user;
		b.car = car;
		b.fromDay = fromDay;
		b.toDay = toDay;
		b.amount = amount;
		b.paymentMode = mode;
		b.isActive = true;

		bookingRepo->addBooking(b);
		cout<<"Car Issued"<<endl;
		return b;
	}

	void returnCar(string bookingId, int currentDay){
		Booking* b = bookingRepo->getBooking(bookingId);
		if(b == NULL){
			cout<<"Booking not found"<<endl;
			return;
		}
		double fineAmt = fine.calculateFine(b->toDay,currentDay);
		if(fineAmt>0)cout<<"Fine: "<<fineAmt<<endl;
		carRepo->markAvailable(b->car.carId);
		b->isActive = false;
		cout<<"Car Returned"<<endl;
	}
};
int main() {
	ios_base::sync_with_stdio(false);
	cin.tie(NULL);

	CarRepository carRepo;
	BookingRepository bookingRepo;
	CarRentalService service{&carRepo,&bookingRepo};

	Car c1 = {"1","Honda City",Sedan,Available};
	Car c2 = {"2","Toyota Fortuner",SUV,Available};

	carRepo.addCar(c1);
	carRepo.addCar(c2);

	User u = {"U1","John","john123@gmail.com","8141234560"};

	Booking booking = service.issueCar(u,c2,1,5,Card);

	service.returnCar(booking.id,7);

	return 0;
}