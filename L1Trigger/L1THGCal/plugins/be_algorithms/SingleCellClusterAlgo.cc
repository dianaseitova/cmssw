#include "L1Trigger/L1THGCal/interface/HGCalTriggerBackendAlgorithmBase.h"
#include "L1Trigger/L1THGCal/interface/fe_codecs/HGCalBestChoiceCodec.h"
#include "DataFormats/ForwardDetId/interface/HGCTriggerDetId.h"

#include "DataFormats/L1THGCal/interface/HGCalCluster.h"

using namespace HGCalTriggerBackend;

class SingleCellClusterAlgo : public Algorithm<HGCalBestChoiceCodec> 
{
    public:

        SingleCellClusterAlgo(const edm::ParameterSet& conf):
            Algorithm<HGCalBestChoiceCodec>(conf),
            cluster_product( new l1t::HGCalClusterBxCollection ){}

        virtual void setProduces(edm::EDProducer& prod) const override final 
        {
            prod.produces<l1t::HGCalClusterBxCollection>(name());
        }

        virtual void run(const l1t::HGCFETriggerDigiCollection& coll,
                const std::unique_ptr<HGCalTriggerGeometryBase>& geom) override final;

        virtual void putInEvent(edm::Event& evt) override final 
        {
            evt.put(cluster_product,name());
        }

        virtual void reset() override final 
        {
            cluster_product.reset( new l1t::HGCalClusterBxCollection );
        }

    private:
        std::auto_ptr<l1t::HGCalClusterBxCollection> cluster_product;

};

/*****************************************************************/
void SingleCellClusterAlgo::run(const l1t::HGCFETriggerDigiCollection& coll,
        const std::unique_ptr<HGCalTriggerGeometryBase>& geom) 
/*****************************************************************/
{
    std::cout<<"####################################\n";
    for( const auto& digi : coll ) 
    {
        HGCalBestChoiceCodec::data_type data;
        data.reset();
        const HGCalDetId& moduleId = digi.getDetId<HGCalDetId>();
        digi.decode(codec_, data);
        std::cout<<"zside="<<moduleId.zside()<<",layer="<<moduleId.layer()<<",module="<<moduleId.wafer()<<"\n";
        int i = 0;
        for(const auto& value : data.payload)
        {
            std::cout<<"  "<<i<<"="<<value<<"\n";
            if(value>0)
            {
                // dummy cluster without position
                // index in module stored as hwEta
                l1t::HGCalCluster cluster( reco::LeafCandidate::LorentzVector(),
                        value, i, 0);
                cluster.setModule(moduleId.wafer());
                cluster.setLayer(moduleId.layer());
                cluster.setSubDet(moduleId.zside()<0 ? 0 : 1);
                cluster_product->push_back(0,cluster);
            }
            i++;
        }

    }
}

DEFINE_EDM_PLUGIN(HGCalTriggerBackendAlgorithmFactory, 
        SingleCellClusterAlgo,
        "SingleCellClusterAlgo");
