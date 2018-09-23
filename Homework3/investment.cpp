class Investment
{
public:
	Investment(string name, int purchase);
	virtual ~Investment() {}

	double purchasePrice() const;
	string name() const;

	virtual bool fungible() const;
	virtual string description() const = 0;
private:
	string m_name;
	int m_purchasePrice;
};

Investment::Investment(string name, int purchase)
	: m_name(name), m_purchasePrice(purchase)
{
	cout << fixed;
	cout.precision(0);
}

double Investment::purchasePrice() const
{
	return m_purchasePrice;
}

string Investment::name() const
{
	return m_name;
}

bool Investment::fungible() const
{
	return false;
}




class Painting : public Investment
{
public:
	Painting(string name, int purchase);
	~Painting();

	string description() const
	{
		return "painting";
	}
};

Painting::Painting(string name, int purchase) : Investment(name, purchase)
{}

Painting::~Painting()
{
	cout << "Destroying " << name() << ", a painting" << endl;
}




class Stock : public Investment
{
public:
	Stock(string name, int purchase, string description);
	~Stock();

	virtual bool fungible() const
	{
		return true;
	}

	string description() const
	{
		return "stock trading as " + m_description;
	}
private:
	string m_description;
};

Stock::Stock(string name, int purchase, string description) : Investment(name, purchase), m_description(description)
{}

Stock::~Stock()
{
	cout << "Destroying " << name() << ", a stock holding" << endl;
}



class House : public Investment
{
public:
	House(string name, int purchase);
	~House();

	string description() const
	{
		return "house";
	}
};

House::House(string name, int purchase) : Investment(name, purchase)
{}

House::~House()
{
	cout << "Destroying the house " << name() << endl;
}



